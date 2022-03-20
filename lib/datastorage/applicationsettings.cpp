/*
 * Copyright 2020 Martin Hoeher <martin@rpdev.net>
 +
 * This file is part of OpenTodoList.
 *
 * OpenTodoList is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * OpenTodoList is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenTodoList.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "applicationsettings.h"

#include <QSettings>
#include <QCoreApplication>

#include "datamodel/library.h"
#include "fileutils.h"
#include "datastorage/librariesitemsquery.h"
#include "datastorage/cache.h"
#include "sync/account.h"
#include "utils/keystore.h"
#include "utils/problemmanager.h"
#include "utils/problem.h"
#include "datastorage/libraryloader.h"
#include "utils/jsonutils.h"

#include <sync/webdavaccount.h>

static Q_LOGGING_CATEGORY(log, "OpenTodoList.ApplicationSettings", QtDebugMsg);

ApplicationSettings::ApplicationSettings(Cache* cache, KeyStore* keyStore,
                                         ProblemManager* problemManager, QObject* parent)
    : QObject(parent),
      m_settings(new QSettings(QSettings::IniFormat, QSettings::UserScope,
                               QCoreApplication::organizationName(),
                               QCoreApplication::applicationName(), this)),
      m_cache(cache),
      m_secrets(),
      m_keyStore(keyStore),
      m_problemManager(problemManager)
{
}

ApplicationSettings::ApplicationSettings(const QString& applicationDir, Cache* cache,
                                         KeyStore* keyStore, ProblemManager* problemManager,
                                         QObject* parent)
    : QObject(parent),
      m_settings(new QSettings(applicationDir + "/appsettings.ini", QSettings::IniFormat, this)),
      m_cache(cache),
      m_secrets(),
      m_keyStore(keyStore),
      m_problemManager(problemManager)
{
}

ApplicationSettings::~ApplicationSettings() {}

QSettings& ApplicationSettings::settings() const
{
    m_settings->sync();
    return *m_settings;
}

QList<QSharedPointer<Library>> ApplicationSettings::librariesFromConfig()
{
    m_settings->sync();
    QList<QSharedPointer<Library>> result;
    auto count = m_settings->beginReadArray("LibraryDirectories");
    for (auto i = 0; i < count; ++i) {
        m_settings->setArrayIndex(i);
        auto directory = m_settings->value("directory").toString();
        directory = FileUtils::fromPersistedPath(directory);
        if (directory != "" && QDir(directory).exists()) {
            auto lib = QSharedPointer<Library>(new Library(directory));
            if (lib->load()) {
                result << lib;
            } else {
                qCWarning(log) << "Failed to load library from directory" << directory;

                if (m_cache) {
                    // Try to restore JSON from cache - fix for
                    // https://gitlab.com/rpdev/opentodolist/issues/222
                    auto query = new LibrariesItemsQuery();
                    query->setIncludeCalculatedValues(false);
                    connect(query, &LibrariesItemsQuery::librariesAvailable,
                            [=](QVariantList libraries) {
                                for (const auto& entry : libraries) {
                                    auto cacheEntry = entry.value<LibraryCacheEntry>();
                                    if (cacheEntry.valid) {
                                        auto lib = Library::decache(cacheEntry);
                                        if (lib->directory() == directory) {
                                            lib->save();
                                            qCWarning(log)
                                                    << "Restored library file in " << directory;
                                        }
                                    }
                                }
                            });
                    m_cache->run(query);
                }
            }
        } else {
            qCWarning(log) << "Library directory" << directory << "does not exist!";
        }
    }
    m_settings->endArray();
    return result;
}

void ApplicationSettings::librariesToConfig(QList<QSharedPointer<Library>> libraries)
{
    m_settings->sync();
    m_settings->beginWriteArray("LibraryDirectories", libraries.length());
    for (auto i = 0; i < libraries.length(); ++i) {
        m_settings->setArrayIndex(i);
        auto dir = libraries[i]->directory();
        dir = FileUtils::toPersistedPath(dir);
        m_settings->setValue("directory", dir);
    }
    m_settings->endArray();
    m_settings->sync();
}

/**
 * @brief Save the Account to the application configuration.
 *
 * This saves the @p account to the application settings.
 */
void ApplicationSettings::saveAccount(Account* account)
{
    m_settings->sync();
    if (account != nullptr) {
        m_settings->beginGroup("Accounts");
        m_settings->beginGroup(account->uid().toString());
        account->save(m_settings);
        m_settings->endGroup();
        m_settings->endGroup();
        m_settings->sync();
        emit accountsChanged();
    }
}

/**
 * @brief Find a library by its @p uid.
 *
 * Returns the library with the given uid or a nullptr if no such library
 * exists.
 */
QSharedPointer<Library> ApplicationSettings::libraryById(const QUuid& uid)
{
    auto libs = librariesFromConfig();
    auto result = std::find_if(libs.begin(), libs.end(),
                               [uid](QSharedPointer<Library> lib) { return lib->uid() == uid; });
    if (result != libs.end()) {
        return *result;
    } else {
        return nullptr;
    }
}

/**
 * @brief Load an account from the app settings.
 *
 * This loads the account with the given @p uid from the settings of
 * the app. If no such account is stored, the function returns a
 * null pointer.
 *
 * @note Owenership goes over to the caller.
 */
Account* ApplicationSettings::loadAccount(const QUuid& uid)
{
    m_settings->sync();
    Account* result = nullptr;
    if (!uid.isNull()) {
        m_settings->beginGroup("Accounts");
        if (m_settings->childGroups().contains(uid.toString())) {
            m_settings->beginGroup(uid.toString());
            result = Account::createAccount(m_settings);
            if (result) {
                result->setUid(uid);
                result->load(m_settings);
                result->setAccountSecrets(m_secrets.value(result->uid().toString()).toString());
                m_settings->endGroup();
            }
        }
        m_settings->endGroup();
    }
    return result;
}

void ApplicationSettings::initialize()
{
    if (m_keyStore) {
        connect(m_keyStore, &KeyStore::credentialsLoaded,
                [=](const QString& key, const QString& value, bool success) {
                    if (success) {
                        if (!m_secrets.contains(key)) {
                            m_secrets.insert(key, value);
                            for (auto lib : librariesFromConfig()) {
                                QScopedPointer<Synchronizer> sync(lib->createSynchronizer());
                                if (sync) {
                                    if (sync->accountUid().isNull()
                                        && sync->uid().toString() == key) {
                                        // Found an old-style synchronizer. Auto-create an account
                                        // from it:
                                        importAccountFromSynchronizer(key, value);
                                    } else {
                                        QSharedPointer<Account> account(
                                                loadAccount(sync->accountUid()));
                                        if (account && account->uid().toString() == key) {
                                            qCDebug(log)
                                                    << "Start-up sync of" << lib << lib->name();
                                            emit libraryLoaded(lib->uid());
                                        }
                                    }
                                }
                            }
                        } else {
                            qCWarning(log) << "Received credentials for a key" << key
                                           << "but we already have"
                                           << "credentials for that one";
                        }
                    } else {
                        // Loading failed - create a problem:
                        qCWarning(log) << "Failed to load credentials for key" << key;
                        QSharedPointer<Account> account(loadAccount(key));
                        if (account && m_problemManager) {
                            Problem problem;
                            problem.setType(Problem::AccountSecretsMissing);
                            problem.setContextObject(account);
                            m_problemManager->addProblem(problem);
                        }
                    }
                });
    }

    loadLibraries();
}

/**
 * @brief Import Accounts by inspecting old-style synchronizers.
 *
 * This is a helper method which is used to automatically create Accounts from existing, old-style
 * synchronizers.
 */
void ApplicationSettings::importAccountsFromSynchronizers()
{
    for (const auto& lib : librariesFromConfig()) {
        QScopedPointer<Synchronizer> sync(lib->createSynchronizer());
        if (sync) {
            if (sync->accountUid().isNull()) {
                // The synchronizer is not connected to an Account. Assume we need to import it.
                m_keyStore->loadCredentials(sync->uid().toString());
            }
        }
    }
}

void ApplicationSettings::importAccountFromSynchronizer(const QString& syncUid,
                                                        const QString& password)
{
    // Load all accounts:
    QList<QSharedPointer<Account>> accounts;
    for (const auto& accountId : accountUids()) {
        accounts << QSharedPointer<Account>(loadAccount(accountId.toString()));
    }

    // Find the synchronizer:
    for (const auto& lib : librariesFromConfig()) {
        QScopedPointer<Synchronizer> sync(lib->createSynchronizer());
        if (sync && sync->uid().toString() == syncUid) {
            // The next should always be the case - before the introduction to accounts, we only
            // had WebDAV synchronizers:
            auto davSync = qobject_cast<WebDAVSynchronizer*>(sync.data());
            if (davSync) {
                // Check if we (meanwhile) have an account that maps to the same URL+username:
                auto existingAccount = std::find_if(
                        accounts.begin(), accounts.end(),
                        [davSync](QSharedPointer<Account> account) {
                            auto webDavAccount = qobject_cast<WebDAVAccount*>(account);
                            return webDavAccount->username() == davSync->username()
                                    && QUrl(webDavAccount->baseUrl()) == davSync->url();
                        });
                if (existingAccount != accounts.end()) {
                    // Connect the synchronizer to the existing account:
                    davSync->setAccountUid((*existingAccount)->uid());
                    davSync->save();
                }

                // The synchronizer does not have an account ID set,
                // assume, it is not yet ported and create one from it:
                auto attrs =
                        JsonUtils::loadMap(sync->directory() + "/" + Synchronizer::SaveFileName);
                WebDAVAccount account;
                account.setDisableCertificateChecks(attrs["disableCertificateCheck"].toBool());
                auto serverType = attrs["serverType"].toString();
                if (serverType == "NextCloud") {
                    account.setType(Account::NextCloud);
                } else if (serverType == "OwnCloud") {
                    account.setType(Account::OwnCloud);
                } else if (serverType == "Generic") {
                    account.setType(Account::WebDAV);
                }
                account.setBaseUrl(attrs["url"].toString());
                account.setUsername(attrs["username"].toString());
                account.setName(account.username() + "@" + QUrl(account.baseUrl()).host());
                account.setPassword(password);
                saveAccount(&account);
                saveAccountSecrets(&account);
                sync->setAccountUid(account.uid());
                sync->save();
                return;
            }
        }
    }
}

void ApplicationSettings::loadLibraries()
{
    qCDebug(log) << "Loading libraries...";
    m_secrets.clear();

    for (auto library : librariesFromConfig()) {
        qCDebug(log) << "Loading library" << library->name() << "from" << library->directory();
        auto loader = new LibraryLoader();
        loader->setDirectory(library->directory());
        loader->setLibraryId(library->uid());
        loader->setCache(m_cache);
        connect(loader, &LibraryLoader::scanFinished, loader, &LibraryLoader::deleteLater);
        loader->scan();
    }

    // Load secrets of all accounts:
    for (const auto& uid : accountUids()) {
        m_keyStore->loadCredentials(uid.toString());
    }

    // Trigger import of old-style synchronizers:
    importAccountsFromSynchronizers();
}

/**
 * @brief Get the list of UIDs of the accounts.
 */
QVariantList ApplicationSettings::accountUids()
{
    m_settings->sync();
    QVariantList result;
    m_settings->beginGroup("Accounts");
    for (const auto& key : m_settings->childGroups()) {
        auto uid = QUuid::fromString(key);
        if (!uid.isNull()) {
            result << uid;
        }
    }
    m_settings->endGroup();
    return result;
}

/**
 * @brief Save the secrets of the @p account.
 */
void ApplicationSettings::saveAccountSecrets(Account* account)
{
    if (account != nullptr) {
        if (m_keyStore) {
            m_keyStore->saveCredentials(account->uid().toString(), account->accountSecrets());
        }
        m_secrets.insert(account->uid().toString(), account->accountSecrets());
        emit accountsChanged();

        // Check if the account we just saved credentials for was previously missing
        // credentials and hence remove the problem:
        if (m_problemManager) {
            for (const auto& problem : m_problemManager->problems()) {
                QSharedPointer<Account> problemAccount =
                        qSharedPointerObjectCast<Account>(problem.contextObject());
                if (problemAccount && problemAccount->uid() == account->uid()) {
                    m_problemManager->removeProblem(problem);
                    break;
                }
            }
        }
    }
}

bool ApplicationSettings::hasSecretsForAccount(Account* account)
{
    if (account) {
        return m_secrets.contains(account->uid().toString());
    } else {
        return false;
    }
}

void ApplicationSettings::setAccountSecret(const QUuid& accountUid, const QString& password)
{
    m_secrets[accountUid.toString()] = password;
}

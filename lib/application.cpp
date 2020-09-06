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

#include "application.h"

#include <QClipboard>
#include <QCoreApplication>
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QGuiApplication>
#include <QJsonDocument>
#include <QProcess>
#include <QScopedPointer>
#include <QStandardPaths>
#include <QTextDocument>
#include <QThreadPool>
#include <QTimer>
#include <QUuid>

#include "datamodel/image.h"
#include "datamodel/note.h"
#include "datamodel/notepage.h"
#include "datamodel/task.h"
#include "datamodel/todo.h"
#include "datamodel/todolist.h"
#include "datastorage/cache.h"
#include "datastorage/deleteitemsquery.h"
#include "datastorage/librariesitemsquery.h"
#include "datastorage/insertorupdateitemsquery.h"
#include "datastorage/libraryloader.h"
#include "sync/account.h"
#include "sync/synchronizer.h"
#include "sync/syncjob.h"
#include "sync/syncrunner.h"
#include "sync/webdavsynchronizer.h"
#include "utils/jsonutils.h"
#include "utils/keystore.h"
#include "utils/directorywatcher.h"
#include "fileutils.h"

static Q_LOGGING_CATEGORY(log, "OpenTodoList.Application", QtDebugMsg)

        /**
         * @brief Constructor.
         *
         * Creates a new Application object. The instance will be a child of the given
         * @p parent.
         */
        Application::Application(QObject* parent)
    : QObject(parent),
      m_settings(new QSettings(QSettings::IniFormat, QSettings::UserScope,
                               QCoreApplication::organizationName(),
                               QCoreApplication::applicationName(), this)),
      m_cache(new Cache(this)),
      m_keyStore(new KeyStore(this)),
      m_problemManager(new ProblemManager(this)),
      m_secrets(),
      m_watchedDirectories(),
      m_librariesWithChanges()
{
    initialize();
}

/**
 * @brief Constructor.
 *
 * This is an overloaded version of the Application constructor. It causes the
 * Application object to store its settings in the given @p applicationDir.
 */
Application::Application(const QString& applicationDir, QObject* parent)
    : QObject(parent),
      m_settings(new QSettings(applicationDir + "/appsettings.ini", QSettings::IniFormat, this)),
      m_cache(new Cache(this)),
      m_keyStore(new KeyStore(this)),
      m_problemManager(new ProblemManager(this)),
      m_secrets(),
      m_watchedDirectories(),
      m_librariesWithChanges()
{
    initialize(applicationDir);
}

/**
 * @brief Shared initialization of constructors.
 */
void Application::initialize(const QString& path)
{
    auto cacheDir = path;
    if (cacheDir.isEmpty()) {
        cacheDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    }
    cacheDir += "/cache";
    {
        QDir dir(cacheDir);
        if (!dir.exists()) {
            if (!dir.mkpath(".")) {
                qWarning() << "Failed to create cache directory";
            }
        }
    }
    m_cache->setCacheDirectory(cacheDir);
    // WA for https://gitlab.com/rpdev/opentodolist/issues/214:
    // Try to use different directories for the cache - required if
    // the user previously ran the app with a different architecture (and
    // hence we cannot re-open the incompatible LMDB DB).
    {
        int i = 0;
        while (!m_cache->open() && i < 10) {
            auto secondaryCacheDir = cacheDir + "-" + QString::number(i++);
            QDir dir(secondaryCacheDir);
            if (!dir.exists()) {
                if (!dir.mkpath(".")) {
                    qWarning() << "Failed to create secondary cache directory" << secondaryCacheDir;
                }
            }
            m_cache->setCacheDirectory(secondaryCacheDir);
        }
    }

    if (!m_cache->isValid()) {
        // If we still were not able to open a cache persistently,
        // resort to using a temporary directory. This means, we would
        // not cache between app restarts, but there should actually not
        // be a reason why we run into this situation (unless we have file
        // system corruptions or something like that).
        m_tmpCacheDir.reset(new QTemporaryDir);
        m_cache->setCacheDirectory(m_tmpCacheDir->path());
        if (!m_cache->open()) {
            qCWarning(log) << "Permanently failed to open a cache directory. "
                              "The app should not crash, but it likely won't "
                              "function as you might expect.";
        }
    }

    connect(m_cache, &Cache::librariesChanged, this, &Application::onLibrariesChanged);

    loadLibraries();

    connect(m_keyStore, &KeyStore::credentialsLoaded,
            [=](const QString& key, const QString& value, bool success) {
                if (success) {
                    if (!m_secrets.contains(key)) {
                        m_secrets.insert(key, value);
                        emit secretsKeysChanged();
                        for (auto lib : librariesFromConfig()) {
                            QScopedPointer<Synchronizer> sync(lib->createSynchronizer());
                            if (sync) {
                                if (sync->accountUid().isNull() && sync->uid().toString() == key) {
                                    // Found an old-style synchronizer. Auto-create an account from
                                    // it:
                                    importAccountFromSynchronizer(key, value);
                                } else {
                                    QSharedPointer<Account> account(
                                            loadAccount(sync->accountUid()));
                                    if (account && account->uid().toString() == key) {
                                        qCDebug(log) << "Start-up sync of" << lib << lib->name();
                                        runSyncForLibrary(lib);
                                    }
                                }
                            }
                        }
                    } else {
                        qCWarning(log)
                                << "Received credentials for a key" << key << "but we already have"
                                << "credentials for that one";
                    }
                } else {
                    // Loading failed - create a problem:
                    qCWarning(log) << "Failed to load credentials for key" << key;
                    QSharedPointer<Account> account(loadAccount(key));
                    if (account) {
                        Problem problem;
                        problem.setType(Problem::AccountSecretsMissing);
                        problem.setContextObject(account);
                        m_problemManager->addProblem(problem);
                    }
                }
            });

    auto syncTimer = new QTimer(this);
    // Check if we need to sync every 5 min
    syncTimer->setInterval(1000 * 60 * 5);
    syncTimer->setSingleShot(false);
    connect(syncTimer, &QTimer::timeout, [=]() {
        for (auto lib : librariesFromConfig()) {
            QScopedPointer<Synchronizer> sync(lib->createSynchronizer());
            if (sync) {
                auto lastSync = sync->lastSync();
                bool runSync = false;
                if (!lastSync.isValid()) {
                    runSync = true;
                } else {
                    auto currentDateTime = QDateTime::currentDateTime();
                    auto diff = currentDateTime.toMSecsSinceEpoch() - lastSync.toMSecsSinceEpoch();
                    if (diff >= (1000 * 60 * 15)) {
                        // Sync every 15min
                        qCDebug(log) << "Library" << lib << lib->name() << "has not been synced for"
                                     << "more than 15min,"
                                     << "starting sync now";
                        runSync = true;
                    }
                }
                if (runSync) {
                    runSyncForLibrary(lib);
                }
            }
        }
    });
    syncTimer->start();
}

QList<QSharedPointer<Library>> Application::librariesFromConfig()
{
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
                                        qCWarning(log) << "Restored library file in " << directory;
                                    }
                                }
                            }
                        });
                m_cache->run(query);
            }
        } else {
            qCWarning(log) << "Library directory" << directory << "does not exist!";
            QDir d(Library::defaultLibrariesLocation());
            for (const auto& entry : d.entryList()) {
                qCWarning(log) << "    " << entry;
            }
        }
    }
    m_settings->endArray();
    return result;
}

void Application::librariesToConfig(QList<QSharedPointer<Library>> libraries)
{
    m_settings->beginWriteArray("LibraryDirectories", libraries.length());
    for (auto i = 0; i < libraries.length(); ++i) {
        m_settings->setArrayIndex(i);
        auto dir = libraries[i]->directory();
        dir = FileUtils::toPersistedPath(dir);
        m_settings->setValue("directory", dir);
    }
    m_settings->endArray();
}

void Application::syncLibrariesWithCache(QList<QSharedPointer<Library>> libraries)
{
    for (auto library : libraries) {
        auto loader = new LibraryLoader(this);
        loader->setCache(m_cache);
        loader->setDirectory(library->directory());
        loader->setLibraryId(library->uid());
        connect(loader, &LibraryLoader::scanFinished, loader, &LibraryLoader::deleteLater);
        loader->scan();
    }
}

/**
 * @brief Internally add the @p library.
 *
 * This method adds the library to the internal list of libraries,
 * ensures it is added to the cache and its content (if already
 * present) loaded fromm disk.
 */
void Application::internallyAddLibrary(Library* library)
{
    auto q = new InsertOrUpdateItemsQuery();
    q->add(library, InsertOrUpdateItemsQuery::Save);
    m_cache->run(q);
    syncLibrary(library);
    library->setCache(m_cache);
    auto libs = librariesFromConfig();
    libs << QSharedPointer<Library>(Library::decache(library->encache()));
    librariesToConfig(libs);
}

/**
 * @brief Check if the given @p uid refers to a known library.
 */
bool Application::isLibraryUid(const QUuid& uid)
{
    return libraryById(uid) != nullptr;
}

/**
 * @brief Find a library by its @p uid.
 *
 * Returns the library with the given uid or a nullptr if no such library
 * exists.
 */
QSharedPointer<Library> Application::libraryById(const QUuid& uid)
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
 * @brief Import Accounts by inspecting old-style synchronizers.
 *
 * This is a helper method which is used to automatically create Accounts from existing, old-style
 * synchronizers.
 */
void Application::importAccountsFromSynchronizers()
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

void Application::importAccountFromSynchronizer(const QString& syncUid, const QString& password)
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
                auto existingAccount =
                        std::find_if(accounts.begin(), accounts.end(),
                                     [davSync](QSharedPointer<Account> account) {
                                         return account->username() == davSync->username()
                                                 && QUrl(account->baseUrl()) == davSync->url();
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
                Account account;
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

template<typename T>
void Application::watchLibraryForChanges(T library)
{
    QScopedPointer<Synchronizer> sync(library->createSynchronizer());
    if (sync == nullptr && library->isValid()) {
        auto watcher = new DirectoryWatcher(this);
        auto directory = library->directory();
        auto uid = library->uid();
        watcher->setDirectory(library->directory());
        m_watchedDirectories[library->directory()] = watcher;
        connect(watcher, &DirectoryWatcher::directoryChanged, [=]() {
            auto loader = new LibraryLoader();
            loader->setCache(m_cache);
            loader->setDirectory(directory);
            loader->setLibraryId(uid);
            connect(loader, &LibraryLoader::scanFinished, loader, &LibraryLoader::deleteLater);
            loader->scan();
        });
    }
}

/**
 * @brief Destructor.
 */
Application::~Application() {}

/**
 * @brief Save the Account to the application configuration.
 *
 * This saves the @p account to the application settings.
 */
void Application::saveAccount(Account* account)
{
    if (account != nullptr) {
        m_settings->beginGroup("Accounts");
        m_settings->beginGroup(account->uid().toString());
        account->save(m_settings);
        m_settings->endGroup();
        m_settings->endGroup();
        emit accountsChanged();
    }
}

/**
 * @brief Save the secrets of the @p account.
 */
void Application::saveAccountSecrets(Account* account)
{
    if (account != nullptr) {
        m_keyStore->saveCredentials(account->uid().toString(), account->password());
        m_secrets.insert(account->uid().toString(), account->password());
        emit accountsChanged();

        // Check if the account we just saved credentials for was previously missing
        // credentials and hence remove the problem:
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

/**
 * @brief Remove the account from the application.
 *
 * This removes the settings of the account. Additionally,
 * all libraries, belonging to that account will be removed, too.
 */
void Application::removeAccount(Account* account)
{
    if (account != nullptr) {
        for (auto& lib : librariesFromConfig()) {
            QScopedPointer<Synchronizer> sync(lib->createSynchronizer());
            if (sync) {
                if (sync->accountUid() == account->uid()) {
                    qCDebug(log) << "Removing library" << lib->name() << lib->uid()
                                 << "as it belongs to account" << account->name() << account->uid()
                                 << "which is removed";
                    deleteLibrary(lib.data());
                }
            }
        }

        m_settings->beginGroup("Accounts");
        m_settings->beginGroup(account->uid().toString());
        for (const auto& key : m_settings->allKeys()) {
            m_settings->remove(key);
        }
        m_keyStore->deleteCredentials(account->uid().toString());
        m_settings->endGroup();
        m_settings->endGroup();
        emit accountsChanged();
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
Account* Application::loadAccount(const QUuid& uid)
{
    Account* result = nullptr;
    if (!uid.isNull()) {
        m_settings->beginGroup("Accounts");
        if (m_settings->childGroups().contains(uid.toString())) {
            m_settings->beginGroup(uid.toString());
            result = new Account();
            result->setUid(uid);
            result->load(m_settings);
            result->setPassword(m_secrets.value(result->uid().toString()).toString());
            m_settings->endGroup();
        }
        m_settings->endGroup();
    }
    return result;
}

/**
 * @brief Get the list of UIDs of the accounts.
 */
QVariantList Application::accountUids()
{
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
 * @brief Create a new local library.
 *
 * This will create a new library, which is stored locally in the
 * default library location.
 */
Library* Application::addLocalLibrary(const QString& name)
{
    Library* result = nullptr;
    auto uid = QUuid::createUuid();
    QDir dir(Library::defaultLibrariesLocation());
    if (!dir.exists() && !dir.mkpath(".")) {
        qCWarning(log) << "Failed to create libraries location in"
                       << Library::defaultLibrariesLocation();
    } else if (dir.mkdir(uid.toString())) {
        auto path = dir.absoluteFilePath(uid.toString());
        result = new Library(path);
        result->setName(name);
        result->setUid(uid);
        result->save();

        watchLibraryForChanges(result);
        internallyAddLibrary(result);
    } else {
        qCWarning(log) << "Failed to create directory for new "
                          "library in "
                       << Library::defaultLibrariesLocation();
    }

    return result;
}

/**
 * @brief Add a local @p directory as a library.
 *
 * This method adds the local @p directory as a library to the application.
 * If the directory points to an existing library location, the library
 * is loaded as-is. Otherwise, a new library is initialized in this
 * directory.
 */
Library* Application::addLibraryDirectory(const QString& directory)
{
    Library* result = nullptr;
    QDir dir(directory);
    if (dir.exists()) {
        if (isLibraryDir(QUrl::fromLocalFile(directory))) {
            result = new Library(directory);
            if (result->load()) {
                auto existingLib = libraryById(result->uid());
                if (existingLib == nullptr) {
                    internallyAddLibrary(result);
                    watchLibraryForChanges(result);
                } else {
                    qCWarning(log) << "Library in" << directory << "is already register";
                    delete result;
                    result = Library::decache(existingLib->encache());
                    result->setCache(m_cache);
                    // Return here - we do not want to add this a second time
                    // to our list:
                    return result;
                }
            } else {
                qCWarning(log) << "Failed to load library from" << directory;
                delete result;
                result = nullptr;
            }
        } else {
            auto uid = QUuid::createUuid();
            QDir dir_(directory);
            if (dir_.mkdir(uid.toString())) {
                result = new Library(dir_.absoluteFilePath(uid.toString()));
                result->setUid(uid);
                internallyAddLibrary(result);
                watchLibraryForChanges(result);
            } else {
                qCWarning(log) << "Failed to create new library folder in" << directory;
            }
        }
    } else {
        qCWarning(log) << "Cannot add" << directory << "as library: It does not exist";
    }
    return result;
}

/**
 * @brief Add a new library to the given account.
 *
 * This methods adds a new library to the @p account. The library is given the specified @p name.
 * Returns the newly created library.
 *
 * @note The caller takes ownership of the returned object.
 */
Library* Application::addNewLibraryToAccount(Account* account, const QString& name)
{
    Library* result = nullptr;
    if (account && !name.isEmpty()) {
        auto uid = QUuid::createUuid();
        QDir dir(Library::defaultLibrariesLocation());
        if (!dir.exists() && !dir.mkpath(".")) {
            qCWarning(log) << "Failed to create libraries location in"
                           << Library::defaultLibrariesLocation();
        } else if (dir.mkdir(uid.toString())) {
            auto path = dir.absoluteFilePath(uid.toString());
            result = new Library(path);
            result->setName(name);
            result->setUid(uid);
            result->save();

            switch (account->type()) {
            case Account::NextCloud:
            case Account::OwnCloud:
            case Account::WebDAV: {
                WebDAVSynchronizer sync;
                sync.setDirectory(result->directory());
                sync.setAccountUid(account->uid());
                sync.setServerType(account->toWebDAVServerType()
                                           .value<WebDAVSynchronizer::WebDAVServerType>());
                sync.setRemoteDirectory("OpenTodoList/" + result->uid().toString() + ".otl");
                sync.setCreateDirs(true);
                sync.save();
                break;
            }
            case Account::Invalid:
                qCWarning(log) << "Invalid account" << account << "passed to addNewLibraryToAccount"
                               << "function";
            }

            watchLibraryForChanges(result);
            internallyAddLibrary(result);
        } else {
            qCWarning(log) << "Failed to create directory for new "
                              "library in "
                           << Library::defaultLibrariesLocation();
        }
    }
    return result;
}

Library* Application::addExistingLibraryToAccount(Account* account,
                                                  const SynchronizerExistingLibrary& library)
{
    Library* result = nullptr;
    if (account && !library.uid().isNull() && !isLibraryUid(library.uid())) {
        auto uid = library.uid();
        QDir dir(Library::defaultLibrariesLocation());
        if (!dir.exists() && !dir.mkpath(".")) {
            qCWarning(log) << "Failed to create libraries location in"
                           << Library::defaultLibrariesLocation();
        } else if (dir.mkdir(uid.toString())) {
            auto path = dir.absoluteFilePath(uid.toString());
            result = new Library(path);
            result->setName(library.name());
            result->setUid(uid);
            result->save();

            switch (account->type()) {
            case Account::NextCloud:
            case Account::OwnCloud:
            case Account::WebDAV: {
                WebDAVSynchronizer sync;
                sync.setDirectory(result->directory());
                sync.setAccountUid(account->uid());
                sync.setServerType(account->toWebDAVServerType()
                                           .value<WebDAVSynchronizer::WebDAVServerType>());
                sync.setRemoteDirectory(library.path());
                sync.save();
                break;
            }
            case Account::Invalid:
                qCWarning(log) << "Invalid account" << account << "passed to addNewLibraryToAccount"
                               << "function";
            }

            watchLibraryForChanges(result);
            internallyAddLibrary(result);
        } else {
            qCWarning(log) << "Failed to create directory for new "
                              "library in "
                           << Library::defaultLibrariesLocation();
        }
    }
    return result;
}

/**
 * @brief Delete the library.
 *
 * This will remove the @p library from the application, i.e.
 * it will be removed from the cache. In addition, if the library
 * is located in the default libraries location, the files on
 * disk stored will also be removed.
 */
void Application::deleteLibrary(Library* library)
{
    if (library != nullptr) {
        auto watcher = m_watchedDirectories.value(library->directory(), nullptr);
        if (m_directoriesWithRunningSync.contains(library->directory())) {
            qCWarning(log) << "Cannot delete a library which is syncing.";
            return;
        }
        if (watcher != nullptr) {
            delete watcher;
            m_watchedDirectories.remove(library->directory());
        }
        auto q = new DeleteItemsQuery();
        q->deleteLibrary(library, library->isInDefaultLocation());
        m_cache->run(q);
        auto libs = librariesFromConfig();
        for (auto lib : libs) {
            if (lib->directory() == library->directory()) {
                libs.removeOne(lib);
                break;
            }
        }
        librariesToConfig(libs);
    }
}

Note* Application::addNote(Library* library, QVariantMap properties)
{
    Note* note = nullptr;
    if (library != nullptr) {
        if (library->isValid()) {
            QDir dir(library->newItemLocation());
            dir.mkpath(".");
            note = new Note(dir);
        } else {
            note = new Note();
        }
        for (auto property : properties.keys()) {
            auto value = properties.value(property);
            note->setProperty(property.toUtf8(), value);
        }
        note->setLibraryId(library->uid());
        auto q = new InsertOrUpdateItemsQuery();
        q->add(note, InsertOrUpdateItemsQuery::CreateNewItem);
        m_cache->run(q);
        note->setCache(m_cache);
    }
    return note;
}

NotePage* Application::addNotePage(Library* library, Note* note, QVariantMap properties)
{
    NotePage* page = nullptr;
    if (library != nullptr && note != nullptr) {
        if (library->isValid()) {
            QDir dir(library->newItemLocation());
            dir.mkpath(".");
            page = new NotePage(dir);
        } else {
            page = new NotePage();
        }
        for (auto property : properties.keys()) {
            auto value = properties.value(property);
            page->setProperty(property.toUtf8(), value);
        }
        page->setNoteUid(note->uid());
        auto q = new InsertOrUpdateItemsQuery();
        q->add(page, InsertOrUpdateItemsQuery::CreateNewItem);
        m_cache->run(q);
        page->setCache(m_cache);
    }
    return page;
}

Image* Application::addImage(Library* library, QVariantMap properties)
{
    Image* image = nullptr;
    if (library != nullptr) {
        if (library->isValid()) {
            QDir dir(library->newItemLocation());
            dir.mkpath(".");
            image = new Image(dir);
        } else {
            image = new Image();
        }
        for (auto property : properties.keys()) {
            auto value = properties.value(property);
            image->setProperty(property.toUtf8(), value);
        }
        image->setLibraryId(library->uid());
        auto q = new InsertOrUpdateItemsQuery();
        q->add(image, InsertOrUpdateItemsQuery::CreateNewItem);
        m_cache->run(q);
        image->setCache(m_cache);
    }
    return image;
}

TodoList* Application::addTodoList(Library* library, QVariantMap properties)
{
    TodoList* todoList = nullptr;
    if (library != nullptr) {
        if (library->isValid()) {
            QDir dir(library->newItemLocation());
            dir.mkpath(".");
            todoList = new TodoList(dir);
        } else {
            todoList = new TodoList();
        }
        for (auto property : properties.keys()) {
            auto value = properties.value(property);
            todoList->setProperty(property.toUtf8(), value);
        }
        todoList->setLibraryId(library->uid());
        auto q = new InsertOrUpdateItemsQuery();
        q->add(todoList, InsertOrUpdateItemsQuery::CreateNewItem);
        m_cache->run(q);
        todoList->setCache(m_cache);
    }
    return todoList;
}

Todo* Application::addTodo(Library* library, TodoList* todoList, QVariantMap properties)
{
    Todo* todo = nullptr;
    if (library != nullptr && todoList != nullptr) {
        if (library->isValid()) {
            QDir dir(library->newItemLocation());
            dir.mkpath(".");
            todo = new Todo(dir);
        } else {
            todo = new Todo();
        }
        for (auto property : properties.keys()) {
            auto value = properties.value(property);
            todo->setProperty(property.toUtf8(), value);
        }
        todo->setTodoListUid(todoList->uid());
        auto q = new InsertOrUpdateItemsQuery();
        q->add(todo, InsertOrUpdateItemsQuery::CreateNewItem);
        m_cache->run(q);
        todo->setCache(m_cache);
    }
    return todo;
}

Task* Application::addTask(Library* library, Todo* todo, QVariantMap properties)
{
    Task* task = nullptr;
    if (library != nullptr && todo != nullptr) {
        if (library->isValid()) {
            QDir dir(library->newItemLocation());
            dir.mkpath(".");
            task = new Task(dir);
        } else {
            task = new Task();
        }
        for (auto property : properties.keys()) {
            auto value = properties.value(property);
            task->setProperty(property.toUtf8(), value);
        }
        task->setTodoUid(todo->uid());
        auto q = new InsertOrUpdateItemsQuery();
        q->add(task, InsertOrUpdateItemsQuery::CreateNewItem);
        m_cache->run(q);
        task->setCache(m_cache);
    }
    return task;
}

void Application::deleteItem(Item* item)
{
    if (item != nullptr) {
        auto q = new DeleteItemsQuery();
        q->deleteItem(item);
        m_cache->run(q);
    }
}

/**
 * @brief Delete all todos from a todo list that are done.
 */
void Application::deleteDoneTodos(TodoList* todoList)
{
    if (todoList != nullptr) {
        auto q = new DeleteItemsQuery();
        q->deleteChildrenOfItem(todoList, [](const Item* item) {
            auto todo = qobject_cast<const Todo*>(item);
            return todo && todo->done();
        });
        m_cache->run(q);
    }
}

/**
 * @brief Delete all taks from a todo that are done.
 */
void Application::deleteDoneTasks(Todo* todo)
{
    if (todo != nullptr) {
        auto q = new DeleteItemsQuery();
        q->deleteChildrenOfItem(todo, [](const Item* item) {
            auto task = qobject_cast<const Task*>(item);
            return task && task->done();
        });
        m_cache->run(q);
    }
}

/**
 * @brief Save a value to the application settings
 *
 * This method is used to save a value to the application settings. Settings
 * can be restored (e.g. when the app restarts).
 */
void Application::saveValue(const QString& name, const QVariant& value)
{
    m_settings->beginGroup("ApplicationSettings");
    m_settings->setValue(name, value);
    m_settings->endGroup();
}

/**
 * @brief Read a persistent application value
 *
 * This method is used to read back persistent application settings which
 * previously have been written using saveValue().
 */
QVariant Application::loadValue(const QString& name, const QVariant& defaultValue)
{
    m_settings->beginGroup("ApplicationSettings");
    QVariant result = m_settings->value(name, defaultValue);
    m_settings->endGroup();
    return result;
}

/**
 * @brief A list of all 3rd party information found in the apps resource system.
 */
QVariant Application::find3rdPartyInfos() const
{
    QDirIterator it(":/", { "3rdpartyinfo.json" }, QDir::Files, QDirIterator::Subdirectories);
    QVariantList result;
    while (it.hasNext()) {
        QString file = it.next();
        bool ok;
        auto map = JsonUtils::loadMap(file, &ok);
        if (ok) {
            result.append(map);
        }
    }
    return result;
}

bool Application::saveTextToFile(const QUrl& fileUrl, const QString& text) const
{
    if (fileUrl.isValid() && fileUrl.isLocalFile()) {
        auto localPath = fileUrl.toLocalFile();
        QFile outputFile(localPath);
        if (outputFile.open(QIODevice::WriteOnly)) {
            outputFile.write(text.toUtf8());
            outputFile.close();
            return true;
        } else {
            qCWarning(log) << "Failed to open" << localPath
                           << "for writing:" << outputFile.errorString();
        }
    }
    return false;
}

/**
 * @brief Convert a URL to a local file name.
 */
QString Application::urlToLocalFile(const QUrl& url) const
{
    return url.toLocalFile();
}

/**
 * @brief Convert a local file name to a url.
 */
QUrl Application::localFileToUrl(const QString& localFile) const
{
    return QUrl::fromLocalFile(localFile);
}

/**
 * @brief Clean the local file path given via the @p url.
 *
 * This method removes extra dots and dotdots from the file path
 * and returns a simplified verion of the input path.
 */
QUrl Application::cleanPath(const QUrl& url) const
{
    auto path = url.toLocalFile();
    path = QDir::cleanPath(path);
    return QUrl::fromLocalFile(path);
}

/**
 * @brief Converts HTML into plain text.
 *
 * This function gets an @p html string as input and returns the text converted
 * to plain text.
 */
QString Application::htmlToPlainText(const QString& html) const
{
    QTextDocument doc;
    doc.setHtml(html);
    return doc.toPlainText();
}

/**
 * @brief Check if a file called @p filename exists.
 */
bool Application::fileExists(const QString& filename) const
{
    return QFile(filename).exists();
}

/**
 * @brief Check if the @p directory exists.
 */
bool Application::directoryExists(const QString& directory) const
{
    return !directory.isEmpty() && QDir(directory).exists();
}

/**
 * @brief Get the basename of the @p filename.
 */
QString Application::basename(const QString& filename) const
{
    return QFileInfo(filename).baseName();
}

/**
 * @brief Test if the @p url points to an existing library directory.
 */
bool Application::isLibraryDir(const QUrl& url) const
{
    bool result = false;
    if (url.isValid()) {
        auto dir = QDir(url.toLocalFile());
        if (dir.exists() && dir.exists(Library::LibraryFileName)) {
            result = true;
        }
    }
    return result;
}

/**
 * @brief Get the name of a library from its directory.
 *
 * This method takes the path to a library and - if the path points to a valid
 * library folder - returns the name of it. When the directory is not a library folder, this
 * returns an empty string.
 */
QString Application::libraryNameFromDir(const QUrl& url) const
{
    if (isLibraryDir(url)) {
        auto path = url.toLocalFile();
        Library lib(path);
        if (lib.load()) {
            return lib.name();
        }
    }
    return QString();
}

/**
 * @brief Get the SHA256 hash over the @p text.
 */
QString Application::sha256(const QString& text) const
{
    return QCryptographicHash::hash(text.toUtf8(), QCryptographicHash::Sha256).toHex();
}

/**
 * @brief Returns the home location of the current user.
 */
QUrl Application::homeLocation() const
{
    QString homeDir = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    return QUrl::fromLocalFile(homeDir);
}

/**
 * @brief Returns true of the folder pointed to by @p url exists.
 */
bool Application::folderExists(const QUrl& url) const
{
    return url.isValid() && QDir(url.toLocalFile()).exists();
}

/**
 * @brief Check if the @p uid refers to an existing library in the app.
 */
bool Application::libraryExists(const QUuid& uid)
{
    return isLibraryUid(uid);
}

/**
 * @brief Start synchronizing the @p library.
 */
void Application::syncLibrary(Library* library)
{
    runSyncForLibrary(library);
}

/**
 * @brief Copy the @p text to the clipboard.
 */
void Application::copyToClipboard(const QString& text)
{
    auto app = dynamic_cast<QGuiApplication*>(qApp);
    if (app != nullptr) {
        auto clipboard = app->clipboard();
        clipboard->setText(text);
    }
}

void Application::clearSyncErrors(Library* library)
{
    if (library) {
        m_syncErrors.remove(library->directory());
        emit syncErrorsChanged();
    }
}

ProblemManager* Application::problemManager() const
{
    return m_problemManager;
}

QVariantMap Application::syncErrors() const
{
    return m_syncErrors;
}

/**
 * @brief A list of secrets keys.
 *
 * @sa Library::synchronizerSecret
 */
QStringList Application::secretsKeys() const
{
    return m_secrets.keys();
}

/**
 * @brief The list of directories in which a sync is currently running.
 */
QStringList Application::directoriesWithRunningSync() const
{
    return m_directoriesWithRunningSync;
}

/**
 * @brief Set the list of directories in which currently a sync is running.
 */
void Application::setDirectoriesWithRunningSync(const QStringList& directoriesWithRunningSync)
{
    if (m_directoriesWithRunningSync != directoriesWithRunningSync) {
        m_directoriesWithRunningSync = directoriesWithRunningSync;
        emit directoriesWithRunningSyncChanged();
    }
}

Cache* Application::cache() const
{
    return m_cache;
}

void Application::loadLibraries()
{
    qCDebug(log) << "Loading libraries...";
    m_secrets.clear();
    emit secretsKeysChanged();

    for (auto library : librariesFromConfig()) {
        qCDebug(log) << "Loading library" << library->name() << "from" << library->directory();
        auto loader = new LibraryLoader();
        loader->setDirectory(library->directory());
        loader->setLibraryId(library->uid());
        loader->setCache(m_cache);
        connect(loader, &LibraryLoader::scanFinished, loader, &LibraryLoader::deleteLater);
        loader->scan();
        watchLibraryForChanges(library);
    }

    // Load secrets of all accounts:
    for (const auto& uid : accountUids()) {
        m_keyStore->loadCredentials(uid.toString());
    }

    // Trigger import of old-style synchronizers:
    importAccountsFromSynchronizers();
}

/**
 * @brief Get the location where libraries are stored by default.
 */
QString Application::librariesLocation() const
{
    return Library::defaultLibrariesLocation();
}

void Application::onLibrarySyncFinished(QString directory)
{
    Library lib(directory);
    if (lib.isValid()) {
        QScopedPointer<Synchronizer> sync(lib.createSynchronizer());
        if (sync) {
            sync->setLastSync(QDateTime::currentDateTime());
            sync->save();
        }

        // Unmark directory as having running sync:
        auto dirs = directoriesWithRunningSync();
        dirs.removeAll(directory);
        setDirectoriesWithRunningSync(dirs);

        if (m_librariesWithChanges.contains(lib.directory())) {
            m_librariesWithChanges.remove(lib.directory());
            runSyncForLibrary(&lib);
        }

        auto libs = librariesFromConfig();
        for (auto libFromConfig : libs) {
            if (libFromConfig->directory() == directory) {
                auto loader = new LibraryLoader();
                loader->setCache(m_cache);
                loader->setLibraryId(libFromConfig->uid());
                loader->setDirectory(directory);
                connect(loader, &LibraryLoader::scanFinished, loader, &LibraryLoader::deleteLater);
                loader->scan();
            }
        }
    }
}

void Application::onLibrarySyncError(QString directory, QString error)
{
    auto errors = m_syncErrors.value(directory, QStringList()).toStringList();
    errors.append(error);
    m_syncErrors[directory] = errors;
    emit syncErrorsChanged();
}

void Application::onLibrariesChanged(QVariantList librariesUids)
{
    auto libs = librariesFromConfig();
    for (auto id : librariesUids) {
        auto uid = id.toUuid();
        for (auto lib : libs) {
            if (uid == lib->uid()) {
                if (m_directoriesWithRunningSync.contains(lib->directory())) {
                    m_librariesWithChanges.insert(lib->directory());
                } else {
                    runSyncForLibrary(lib);
                }
            }
        }
    }
}

template<typename T>
void Application::runSyncForLibrary(T library)
{
    if (library != nullptr && library->isValid()) {
        if (!m_directoriesWithRunningSync.contains(library->directory())) {
            QScopedPointer<Synchronizer> sync(library->createSynchronizer());
            if (!sync) {
                return;
            }
            QSharedPointer<Account> account(loadAccount(sync->accountUid()));
            if (!account || !m_secrets.contains(account->uid().toString())) {
                return;
            }
            setDirectoriesWithRunningSync(directoriesWithRunningSync() << library->directory());
            m_syncErrors.remove(library->directory());
            emit syncErrorsChanged();
            auto job = new SyncJob(library->directory(), account);
            connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit, job,
                    &SyncJob::stop);
            connect(job, &SyncJob::syncFinished, this, &Application::onLibrarySyncFinished,
                    Qt::QueuedConnection);
            connect(job, &SyncJob::syncError, this, &Application::onLibrarySyncError,
                    Qt::QueuedConnection);
            auto runner = new SyncRunner(job);
            QThreadPool::globalInstance()->start(runner);
        }
    }
}

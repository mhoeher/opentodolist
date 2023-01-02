/*
 * Copyright 2020-2021 Martin Hoeher <martin@rpdev.net>
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

#include "backgroundservice.h"

#include <QCoreApplication>
#include <QLoggingCategory>
#include <QThreadPool>

#include "datamodel/library.h"
#include "datastorage/applicationsettings.h"
#include "datastorage/cache.h"
#include "datastorage/deleteitemsquery.h"
#include "datastorage/libraryloader.h"
#include "sync/account.h"
#include "sync/synchronizer.h"
#include "sync/syncjob.h"
#include "sync/syncrunner.h"
#include "utils/directorywatcher.h"
#include "utils/keystore.h"

static Q_LOGGING_CATEGORY(log, "OpenTodoList.BackgroundService", QtDebugMsg);

BackgroundService::BackgroundService(Cache* cache, QObject* parent)
    : BackgroundServiceSource(parent),
      m_threadPool(new QThreadPool(this)),
      m_keyStore(new KeyStore(this)),
      m_appSettings(new ApplicationSettings(cache, m_keyStore, nullptr, this)),
      m_cache(cache),
      m_syncDirs(),
      m_watchedDirectories(),
      m_accountsCheckingConnectivity()
{
    qCDebug(log) << "Creating OpenTodoList BackgroundService object";
    connect(m_appSettings, &ApplicationSettings::libraryLoaded, this,
            &BackgroundService::syncLibrary);
    connect(m_cache, &Cache::dataChanged, this, &BackgroundService::propagateCacheDataChanged);
    connect(m_cache, &Cache::librariesChanged, this,
            &BackgroundService::propagateCacheLibrariesChanged);

    qCDebug(log) << "Initializing app settings";
    m_appSettings->initialize();

    // Bind the app's aboutToExit() to the service's one:
    connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit, this,
            &BackgroundService::serviceAboutToExit);

    auto syncTimer = new QTimer(this);
    // Check if we need to sync every 5 min
    syncTimer->setInterval(1000 * 60 * 5);
    syncTimer->setSingleShot(false);
    connect(syncTimer, &QTimer::timeout, this, [=]() {
        for (auto lib : m_appSettings->librariesFromConfig()) {
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
                    syncLibrary(lib->uid());
                }
            }
        }
    });
    syncTimer->start();

    for (const auto& library : m_appSettings->librariesFromConfig()) {
        watchLibraryForChanges(library);
    }

    auto connectivityCheckTimer = new QTimer(this);
    connectivityCheckTimer->setInterval(15 * 60 * 1000);
    connectivityCheckTimer->setSingleShot(false);
    connect(connectivityCheckTimer, &QTimer::timeout, this,
            &BackgroundService::checkConnectivityOfAccounts);
    connectivityCheckTimer->start();
    QTimer::singleShot(5000, this, &BackgroundService::checkConnectivityOfAccounts);
}

BackgroundService::~BackgroundService()
{
    m_threadPool->waitForDone();
    delete m_threadPool;
}

void BackgroundService::syncLibrary(const QUuid& libraryUid)
{
    qCDebug(log) << "Sync of library with uid" << libraryUid << "requested";
    auto library = m_appSettings->libraryById(libraryUid);
    if (library != nullptr && library->isValid()) {
        if (!m_syncDirs.contains(library->directory())) {
            QScopedPointer<Synchronizer> sync(library->createSynchronizer());
            if (!sync) {
                return;
            }
            QSharedPointer<Account> account(m_appSettings->loadAccount(sync->accountUid()));
            if (!account || !m_appSettings->hasSecretsForAccount(account.data())) {
                return;
            }
            if (account->needConnectivityCheck()) {
                // The account indicated that we need to run a connectitivty check.
                // In this case, don't sync and instead trigger the check. It will
                // cause credentials to be renewed and in turn trigger a sync later.
                checkConnectivityOfAccount(m_appSettings->loadAccount(account->uid()));
                return;
            }
            QSharedPointer<SyncJob> job(new SyncJob(library->directory(), account));
            m_syncDirs[library->directory()] = { job, libraryUid, NoSyncInfoFlags };
            connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit, job.data(),
                    &SyncJob::stop);
            connect(job.data(), &SyncJob::syncFinished, this, &BackgroundService::onSyncFinished,
                    Qt::QueuedConnection);
            connect(job.data(), &SyncJob::syncError, this, &BackgroundService::onSyncError,
                    Qt::QueuedConnection);
            connect(job.data(), &SyncJob::progress, this, &BackgroundService::onSyncProgress,
                    Qt::QueuedConnection);
            auto runner = new SyncRunner(job);
            m_threadPool->start(runner);
            emit librarySyncStarted(libraryUid);
            qCDebug(log) << "Sync of library with uid" << libraryUid << "triggered";
        }
    } else {
        qCWarning(log) << "Library" << libraryUid << "not found";
    }
}

void BackgroundService::deleteLibrary(const QUuid& libraryUid)
{
    qCDebug(log) << "Delete of library" << libraryUid << "requested";
    auto library = m_appSettings->libraryById(libraryUid);
    if (library) {
        if (m_watchedDirectories.contains(library->directory())) {
            delete m_watchedDirectories[library->directory()];
            m_watchedDirectories.remove(library->directory());
        }
        if (m_syncDirs.contains(library->directory())) {
            qCDebug(log) << "Library" << libraryUid << "is currently syncing - mark for removal";
            auto& entry = m_syncDirs[library->directory()];
            entry.job->stop();
            entry.flags = entry.flags | DeleteAfterSync;
        } else {
            qCDebug(log) << "Immediately deleting library" << libraryUid;
            doDeleteLibrary(libraryUid);
        }
    } else {
        qCWarning(log) << "Library" << libraryUid << "not found";
    }
}

void BackgroundService::setAccountSecret(const QUuid& accountUid, const QString& password)
{
    qCDebug(log) << "Received account update for" << accountUid;
    m_appSettings->setAccountSecret(accountUid, password);

    auto account = m_appSettings->loadAccount(accountUid);
    if (account) {
        // Startup connectivity check
        checkConnectivityOfAccount(account);
        syncLibrariesOfAccount(accountUid);
    }
}

void BackgroundService::watchLibraryDirectory(const QUuid& libraryUid)
{
    qCDebug(log) << "Request to watch library" << libraryUid;
    auto lib = m_appSettings->libraryById(libraryUid);
    if (lib) {
        watchLibraryForChanges(lib);
    }
}

void BackgroundService::notifyCacheDataChanged(const QUuid& appInstanceUid)
{
    qCDebug(log) << "Received cache data changed notification from" << appInstanceUid;
    emit cacheDataChanged(appInstanceUid);
}

void BackgroundService::notifyCacheLibrariesChanged(const QVariantList& libraryUids,
                                                    const QUuid& appInstanceUid)
{
    qCDebug(log) << "Received lib cache data changed notification from" << appInstanceUid;
    emit cacheLibrariesChanged(libraryUids, appInstanceUid);
}

void BackgroundService::onSyncFinished(const QString& libraryDirectory)
{
    qCDebug(log) << "Syncing directory" << libraryDirectory << "finished";
    if (m_syncDirs.contains(libraryDirectory)) {
        auto entry = m_syncDirs[libraryDirectory];
        emit librarySyncFinished(entry.libraryUid);
        m_syncDirs.remove(libraryDirectory);
        if (entry.flags.testFlag(DeleteAfterSync)) {
            doDeleteLibrary(entry.libraryUid);
        } else {
            // Load changes from disk:
            auto lib = m_appSettings->libraryById(entry.libraryUid);
            if (lib) {
                auto loader = new LibraryLoader();
                loader->setCache(m_cache);
                loader->setLibraryId(lib->uid());
                loader->setDirectory(lib->directory());
                connect(loader, &LibraryLoader::scanFinished, loader, &LibraryLoader::deleteLater);
                loader->scan();
            }
        }
    }
}

void BackgroundService::onSyncError(const QString& libraryDirectory, const QString& error)
{
    qCDebug(log) << "Error syncing" << libraryDirectory << ":" << error;
    if (m_syncDirs.contains(libraryDirectory)) {
        const auto& entry = m_syncDirs[libraryDirectory];
        emit librarySyncError(entry.libraryUid, error);
    }
}

void BackgroundService::onSyncProgress(const QString& libraryDirectory, int value)
{
    if (m_syncDirs.contains(libraryDirectory)) {
        const auto& entry = m_syncDirs[libraryDirectory];
        emit librarySyncProgress(entry.libraryUid, value);
    }
}

void BackgroundService::doDeleteLibrary(const QUuid& libraryUid)
{
    auto library = m_appSettings->libraryById(libraryUid);
    if (library) {
        auto q = new DeleteItemsQuery();
        q->deleteLibrary(library.data(), library->isInDefaultLocation());
        m_cache->run(q);
        auto libs = m_appSettings->librariesFromConfig();
        for (auto lib : libs) {
            if (lib->directory() == library->directory()) {
                libs.removeOne(lib);
                break;
            }
        }
        m_appSettings->librariesToConfig(libs);
        emit libraryDeleted(libraryUid, library->directory());
    } else {
        qCWarning(log) << "Library" << libraryUid << "not found";
    }
}

void BackgroundService::watchLibraryForChanges(QSharedPointer<Library> library)
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

void BackgroundService::propagateCacheDataChanged()
{
    emit cacheDataChanged(QUuid());
}

void BackgroundService::propagateCacheLibrariesChanged(const QVariantList& libraryUids)
{
    emit cacheLibrariesChanged(libraryUids, QUuid());
}

/**
 * @brief Start the connectivity check of the account.
 *
 * This starts the check for connectivity of the @p account. Note that this method
 * assumes ownership of the account. Calling code must not access the account anymore.
 */
void BackgroundService::checkConnectivityOfAccount(Account* account)
{
    if (m_accountsCheckingConnectivity.contains(account->uid())) {
        // A check already runs - skip
        qCDebug(log) << "Account" << account->uid() << "still busy checking connectivity...";
        delete account;
        return;
    }

    if (!m_appSettings->hasSecretsForAccount(account)) {
        qCDebug(log) << "No secrets for" << account->uid()
                     << "loaded yet - skipping connectivity check";
        delete account;
        return;
    }

    qCDebug(log) << "Checking connectivity for account" << account;
    connect(account, &Account::connectivityCheckFinished, this, [=](bool connected) {
        // TODO: Do something useful with the connected state ;-)
        qCDebug(log) << "Account" << account->uid() << "connected:" << connected;
        m_accountsCheckingConnectivity.remove(account->uid());
        account->deleteLater();
    });
    connect(account, &Account::accountSecretsChanged, this, [=]() {
        // The secrets of the account changed - e.g. access tokens. Propagate to GUI
        qCDebug(log) << "Secrets of account" << account->uid() << "changed - need to save them";
        m_appSettings->saveAccount(account);
        m_appSettings->saveAccountSecrets(account);
        emit accountSecretChanged(account->uid(), account->accountSecrets());

        // For all libraries belonging to this account, trigger a sync - in case the app was not
        // used for a while, we otherwise might have these libraries sitting around with a sync
        // error.
        syncLibrariesOfAccount(account->uid());
    });
    m_accountsCheckingConnectivity.insert(account->uid());
    account->checkConnectivity();
}

/**
 * @brief Sync all libraries belonging to the account with the given @p uid.
 */
void BackgroundService::syncLibrariesOfAccount(const QUuid& uid)
{
    for (const auto& lib : m_appSettings->librariesFromConfig()) {
        QScopedPointer<Synchronizer> sync(lib->createSynchronizer());
        if (sync) {
            if (sync->accountUid() == uid) {
                syncLibrary(lib->uid());
            }
        }
    }
}

/**
 * @brief Triggers a connectivity check for all accounts.
 */
void BackgroundService::checkConnectivityOfAccounts()
{
    const auto uids = m_appSettings->accountUids();
    for (const auto& uid : uids) {
        auto account = m_appSettings->loadAccount(uid.toUuid());
        if (account) {
            checkConnectivityOfAccount(account);
        }
    }
}

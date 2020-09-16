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

#include "backgroundservice.h"

#include <QCoreApplication>
#include <QLoggingCategory>
#include <QThreadPool>

#include "datamodel/library.h"
#include "datastorage/applicationsettings.h"
#include "datastorage/cache.h"
#include "datastorage/deleteitemsquery.h"
#include "sync/account.h"
#include "sync/synchronizer.h"
#include "sync/syncjob.h"
#include "sync/syncrunner.h"
#include "utils/keystore.h"

static Q_LOGGING_CATEGORY(log, "OpenTodoList.Application", QtDebugMsg);

BackgroundService::BackgroundService(Cache* cache, QObject* parent)
    : BackgroundServiceSource(parent),
      m_keyStore(new KeyStore(this)),
      m_appSettings(new ApplicationSettings(cache, m_keyStore, nullptr, this)),
      m_cache(cache)
{
    connect(m_appSettings, &ApplicationSettings::libraryLoaded, this,
            &BackgroundService::syncLibrary);
    connect(m_cache, &Cache::dataChanged, this, &BackgroundService::cacheDataChanged);
    connect(m_cache, &Cache::librariesChanged, this, &BackgroundService::cacheLibrariesChanged);

    m_appSettings->initialize();
}

BackgroundService::~BackgroundService() {}

void BackgroundService::syncLibrary(const QUuid& libraryUid)
{
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
            QSharedPointer<SyncJob> job(new SyncJob(library->directory(), account));
            m_syncDirs[library->directory()] = { job, libraryUid, NoSyncInfoFlags };
            connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit, job.data(),
                    &SyncJob::stop);
            connect(job.data(), &SyncJob::syncFinished, this, &BackgroundService::onSyncFinished,
                    Qt::QueuedConnection);
            connect(job.data(), &SyncJob::syncError, this, &BackgroundService::onSyncError,
                    Qt::QueuedConnection);
            auto runner = new SyncRunner(job);
            QThreadPool::globalInstance()->start(runner);
            emit librarySyncStarted(libraryUid);
        }
    }
}

void BackgroundService::deleteLibrary(const QUuid& libraryUid)
{
    auto library = m_appSettings->libraryById(libraryUid);
    if (library) {
        if (m_syncDirs.contains(library->directory())) {
            auto& entry = m_syncDirs[library->directory()];
            entry.job->stop();
            entry.flags = entry.flags | DeleteAfterSync;
        } else {
            doDeleteLibrary(libraryUid);
        }
    }
}

void BackgroundService::setAccountSecret(const QUuid& accountUid, const QString& password)
{
    m_appSettings->setAccountSecret(accountUid, password);
}

void BackgroundService::onSyncFinished(const QString& libraryDirectory)
{
    if (m_syncDirs.contains(libraryDirectory)) {
        auto entry = m_syncDirs[libraryDirectory];
        emit librarySyncFinished(entry.libraryUid);
        m_syncDirs.remove(libraryDirectory);
        if (entry.flags.testFlag(DeleteAfterSync)) {
            doDeleteLibrary(entry.libraryUid);
        }
    }
}

void BackgroundService::onSyncError(const QString& libraryDirectory, const QString& error)
{
    if (m_syncDirs.contains(libraryDirectory)) {
        const auto& entry = m_syncDirs[libraryDirectory];
        emit librarySyncError(entry.libraryUid, error);
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
    }
}

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

#include "syncjob.h"

#include <QLoggingCategory>

#include "synchronizer.h"

static Q_LOGGING_CATEGORY(log, "OpenTodoList.SyncJob", QtWarningMsg);

/**
 * @brief Create a new sync job.
 *
 * This creates a new sync job for the library which
 * stores it's data in the @p libraryDirectory. The @p secret is
 * the passphrase or key required by the library's synchronizer
 * to talk to the backend server (if required). The @p uid is the uid of the library for which we
 * run the sync.
 *
 * @note The library is only used as a handle. None of it's methods
 * are called, as it might get deleted while the sync is running
 * in the background.
 */
SyncJob::SyncJob(const QString& libraryDirectory, QSharedPointer<Account> account, QObject* parent)
    : QObject(parent), m_libraryDirectory(libraryDirectory), m_account(account)
{
}

/**
 * @brief Run the synchronization.
 */
void SyncJob::execute()
{
    qCDebug(log) << "Sync job" << this << "is executed";
    if (!m_libraryDirectory.isEmpty()) {
        QScopedPointer<Synchronizer> sync(Synchronizer::fromDirectory(m_libraryDirectory));
        if (sync) {
            qCDebug(log) << "Loading sync log";
            sync->loadLog();
            qCDebug(log) << "Setting account data";
            sync->setAccount(m_account.data());
            qCDebug(log) << "Setting up stop requested";
            connect(this, &SyncJob::stopRequested, sync.data(), &Synchronizer::stopSync,
                    Qt::QueuedConnection);
            qCDebug(log) << "Setting up error reporting";
            connect(sync.data(), &Synchronizer::syncError, this, &SyncJob::onSyncError);
            qCDebug(log) << "Start sync";
            sync->synchronize();
            qCDebug(log) << "Saving sync log";
            sync->saveLog();
        }
    } else {
        qCWarning(log) << "Empty library directory given to sync job" << this;
    }
    emit syncFinished(m_libraryDirectory);
}

void SyncJob::stop()
{
    emit stopRequested();
}

void SyncJob::onSyncError(const QString& error)
{
    emit syncError(m_libraryDirectory, error);
}

/*
 * Copyright 2022 Martin Hoeher <martin@rpdev.net>
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

#include "dropboxsynchronizer.h"

#include <SynqClient/directorysynchronizer.h>
#include <SynqClient/dropboxjobfactory.h>
#include <SynqClient/sqlsyncstatedatabase.h>

#include "dropboxaccount.h"

static Q_LOGGING_CATEGORY(log, "OpenTodoList.DropboxSynchronizer", QtDebugMsg);

DropboxSynchronizer::DropboxSynchronizer(QObject* parent)
    : Synchronizer(parent), m_accessToken(), m_stopRequested(false)
{
}

void DropboxSynchronizer::synchronize()
{
    if (!directory().isEmpty() && !synchronizing()) {
        SynqClient::DirectorySynchronizer sync;
        sync.setRemoteDirectoryPath(remoteDirectory());
        sync.setLocalDirectoryPath(directory());
        sync.setSyncConflictStrategy(SynqClient::SyncConflictStrategy::RemoteWins);
        connect(&sync, &SynqClient::DirectorySynchronizer::logMessageAvailable, this,
                [=](SynqClient::SynchronizerLogEntryType type, const QString& message) {
                    switch (type) {
                    case SynqClient::SynchronizerLogEntryType::Information:
                        writeLog(Synchronizer::Debug) << message;
                        break;
                    case SynqClient::SynchronizerLogEntryType::Warning:
                        writeLog(Synchronizer::Warning) << message;
                        break;
                    case SynqClient::SynchronizerLogEntryType::Error:
                        writeLog(Synchronizer::Error) << message;
                        break;
                    case SynqClient::SynchronizerLogEntryType::LocalMkDir:
                        writeLog(Synchronizer::LocalMkDir) << message;
                        break;
                    case SynqClient::SynchronizerLogEntryType::RemoteMkDir:
                        writeLog(Synchronizer::RemoteMkDir) << message;
                        break;
                    case SynqClient::SynchronizerLogEntryType::LocalDelete:
                        writeLog(Synchronizer::LocalDelete) << message;
                        break;
                    case SynqClient::SynchronizerLogEntryType::RemoteDelete:
                        writeLog(Synchronizer::RemoteDelete) << message;
                        break;
                    case SynqClient::SynchronizerLogEntryType::Upload:
                        writeLog(Synchronizer::Upload) << message;
                        break;
                    case SynqClient::SynchronizerLogEntryType::Download:
                        writeLog(Synchronizer::Download) << message;
                        break;
                    }
                });
        connect(&sync, &SynqClient::DirectorySynchronizer::progress, this,
                &DropboxSynchronizer::progress);

        SynqClient::DropboxJobFactory factory;
        QNetworkAccessManager nam;
        nam.setRedirectPolicy(QNetworkRequest::NoLessSafeRedirectPolicy);
        factory.setNetworkAccessManager(&nam);
        factory.setUserAgent(Synchronizer::HTTPUserAgent);
        factory.setToken(m_accessToken);
        sync.setJobFactory(&factory);

        SynqClient::SQLSyncStateDatabase db(directory() + "/.otldropboxsync.db");
        sync.setSyncStateDatabase(&db);

        static QRegularExpression pathRe(
                R"(^\/(library\.json|\d\d\d\d(\/\d\d?(\/[^\.]+\.[a-zA-Z\.]+)?)?)?$)");
        sync.setFilter([=](const QString& path, const SynqClient::FileInfo&) {
            auto result = pathRe.match(path).hasMatch();
            return result;
        });

        QEventLoop loop;
        connect(&sync, &SynqClient::DirectorySynchronizer::finished, &loop, &QEventLoop::quit);
        connect(this, &DropboxSynchronizer::stopRequested, &sync,
                &SynqClient::DirectorySynchronizer::stop);

        setSynchronizing(true);
        m_stopRequested = false;

        sync.start();
        loop.exec();

        if (sync.error() != SynqClient::SynchronizerError::NoError) {
            emit syncError(sync.errorString());
        }

        setSynchronizing(false);
    }
}

void DropboxSynchronizer::stopSync()
{
    qCWarning(::log) << "Stopping Dropbox sync";
    m_stopRequested = true;
    emit stopRequested();
}

QVariantMap DropboxSynchronizer::toMap() const
{
    return Synchronizer::toMap();
}

void DropboxSynchronizer::fromMap(const QVariantMap& map)
{
    Synchronizer::fromMap(map);
}

void DropboxSynchronizer::setAccount(Account* account)
{
    auto dropboxAccount = qobject_cast<DropboxAccount*>(account);
    q_check_ptr(dropboxAccount);

    setAccessToken(dropboxAccount->accessToken());
}

const QString& DropboxSynchronizer::accessToken() const
{
    return m_accessToken;
}

void DropboxSynchronizer::setAccessToken(const QString& newAccessToken)
{
    if (m_accessToken == newAccessToken)
        return;
    m_accessToken = newAccessToken;
    emit accessTokenChanged();
}

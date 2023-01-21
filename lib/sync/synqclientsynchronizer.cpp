/*
 * Copyright 2023 Martin Hoeher <martin@rpdev.net>
 *
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

#include "synqclientsynchronizer.h"

#include <QLoggingCategory>
#include <QEventLoop>
#include <QRegularExpression>

#include <SynqClient/directorysynchronizer.h>

static Q_LOGGING_CATEGORY(log, "OpenTodoList.SynqClientSynchronizer", QtDebugMsg);

SynqClientSynchronizer::SynqClientSynchronizer(QObject* parent) : Synchronizer { parent } {}

void SynqClientSynchronizer::stopSync()
{
    qCWarning(::log) << "Stopping SynqClient sync";
    emit stopRequested();
}

void SynqClientSynchronizer::setupDirectorySynchronizer(SynqClient::DirectorySynchronizer& sync)
{
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
            &SynqClientSynchronizer::progress);

    static QRegularExpression pathRe(
            R"(^\/(library\.json|\d\d\d\d(\/\d\d?(\/[^\.]+\.[a-zA-Z\.]+)?)?)?$)");
    sync.setFilter([=](const QString& path, const SynqClient::FileInfo&) {
        auto result = pathRe.match(path).hasMatch();
        return result;
    });
}

void SynqClientSynchronizer::runDirectorySynchronizer(SynqClient::DirectorySynchronizer& sync)
{
    QEventLoop loop;
    connect(&sync, &SynqClient::DirectorySynchronizer::finished, &loop, &QEventLoop::quit);
    connect(this, &SynqClientSynchronizer::stopRequested, &sync,
            &SynqClient::DirectorySynchronizer::stop);

    setSynchronizing(true);

    sync.start();
    loop.exec();

    if (sync.error() != SynqClient::SynchronizerError::NoError) {
        emit syncError(sync.errorString());
    }

    setSynchronizing(false);
}

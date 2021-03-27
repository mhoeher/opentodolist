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

#ifndef SERVICE_BACKGROUNDSERVICE_H_
#define SERVICE_BACKGROUNDSERVICE_H_

#include <QPointer>
#include <QStringList>

#include "rep_backgroundservice_source.h"

class ApplicationSettings;
class Cache;
class DirectoryWatcher;
class KeyStore;
class Library;
class QThreadPool;
class SyncJob;

class BackgroundService : public BackgroundServiceSource
{
public:
    explicit BackgroundService(Cache* cache, QObject* parent = nullptr);
    ~BackgroundService() override;

public slots:
    // BackgroundServiceSource interface
    void syncLibrary(const QUuid& libraryUid) override;
    void deleteLibrary(const QUuid& libraryUid) override;
    void setAccountSecret(const QUuid& accountUid, const QString& password) override;
    void watchLibraryDirectory(const QUuid& libraryUid) override;
    void notifyCacheDataChanged(const QUuid& appInstanceUid) override;
    void notifyCacheLibrariesChanged(const QVariantList& libraryUids,
                                     const QUuid& appInstanceUid) override;

private:
    enum SyncInfoFlag { NoSyncInfoFlags = 0, DeleteAfterSync = 0x01 };

    Q_DECLARE_FLAGS(SyncInfoFlags, SyncInfoFlag);
    Q_FLAG(SyncInfoFlags);

    Q_FLAGS(SyncInfoFlags);

    struct SyncInfo
    {
        QSharedPointer<SyncJob> job;
        QUuid libraryUid;
        SyncInfoFlags flags;
    };

    QThreadPool* m_threadPool;
    KeyStore* m_keyStore;
    ApplicationSettings* m_appSettings;
    QPointer<Cache> m_cache;
    QMap<QString, SyncInfo> m_syncDirs;
    QMap<QString, DirectoryWatcher*> m_watchedDirectories;

    void onSyncFinished(const QString& libraryDirectory);
    void onSyncError(const QString& libraryDirectory, const QString& error);
    void onSyncProgress(const QString& libraryDirectory, int value);
    void doDeleteLibrary(const QUuid& libraryUid);
    void watchLibraryForChanges(QSharedPointer<Library> library);
    void propagateCacheDataChanged();
    void propagateCacheLibrariesChanged(const QVariantList& libraryUids);
};

#endif // SERVICE_BACKGROUNDSERVICE_H_

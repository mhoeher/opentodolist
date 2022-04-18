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

#ifndef SYNC_SYNCHRONIZER_H_
#define SYNC_SYNCHRONIZER_H_

#include <QDebug>
#include <QDateTime>
#include <QLoggingCategory>
#include <QObject>
#include <QUuid>
#include <QVariantMap>

class Account;

/**
 * @brief Base class for file synchronization.
 *
 * The Synchronizer class serves as abstract base class for file based
 * synchronization. It is intended to be used to synchronize the files of
 * a Library with an arbitrary backend service (such as a web service). The Synchronizer
 * provides the abstract interface required for such tasks. Concrete sub-classes are then
 * introduced to implement the actual synchronization with various backends.
 */
class Synchronizer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool synchronizing READ synchronizing NOTIFY synchronizingChanged)
    Q_PROPERTY(QString directory READ directory WRITE setDirectory NOTIFY directoryChanged)
    Q_PROPERTY(QString remoteDirectory READ remoteDirectory WRITE setRemoteDirectory NOTIFY
                       remoteDirectoryChanged)
    Q_PROPERTY(bool isNull READ isNull NOTIFY directoryChanged)
    Q_PROPERTY(QString type READ type CONSTANT)
public:
    enum LogType {
        Debug,
        Warning,
        Error,
        LocalMkDir,
        RemoteMkDir,
        LocalDelete,
        RemoteDelete,
        Download,
        Upload
    };

    Q_ENUM(LogType);

    static const QString HTTPUserAgent;

    struct LogEntry
    {
        QDateTime time;
        LogType type;
        QString message;
    };

    static const QString SaveFileName;
    static const QString LogFileName;
    static const int MaxLogEntries = 5000;

    explicit Synchronizer(QObject* parent = nullptr);
    virtual ~Synchronizer();

    bool synchronizing() const;

    QString directory() const;
    void setDirectory(const QString& directory);

    QString remoteDirectory() const;
    void setRemoteDirectory(const QString& remoteDirectory);

    bool isNull() const;

    Q_INVOKABLE bool save() const;
    void restore();

    QString type() const;

    static Synchronizer* fromDirectory(const QString& dir, QObject* parent = nullptr);

    /**
     * @brief Synchronize the local directory with the backend.
     *
     * This method starts a new synchronization with with the backend. Sub-classes
     * are supposed to implement this method following these steps:
     */
    virtual void synchronize() = 0;

    /**
     * @brief Stop the currently running synchronization procedure.
     *
     * This method stops the currently running synchronization. The default
     * implementation of this method does nothing. Implementations of the
     * Synchronizer interface should implement specific means to stop
     * a synchronization - this might be crucial for some platforms the app
     * runs on, where upon a request for the app to terminate, this must
     * be carried out as fast as possible.
     */
    virtual void stopSync() {}

    virtual QVariantMap toMap() const;
    virtual void fromMap(const QVariantMap& map);

    bool findingLibraries() const;

    bool createDirs() const;
    void setCreateDirs(bool createDirs);

    QUuid uid() const;

    QDateTime lastSync() const;

    void setLastSync(const QDateTime& lastSync);

    // For logging:
    QDebug debug();
    QDebug warning();
    QDebug error();
    QDebug writeLog(LogType type);

    QList<LogEntry> log() const;
    bool loadLog();
    bool saveLog();

    QUuid accountUid() const;
    void setAccountUid(const QUuid& accountUid);

    virtual void setAccount(Account* account);

signals:

    void synchronizingChanged();
    void directoryChanged();
    void remoteDirectoryChanged();
    void secretChanged();
    void syncError(const QString& message);
    void progress(int value);

public slots:

protected:
    void setSynchronizing(bool synchronizing);

private:
    QUuid m_uuid;
    QUuid m_accountUid;
    bool m_synchronizing;
    bool m_creatingDirectory;
    bool m_createDirs;
    QString m_directory;
    QString m_remoteDirectory;
    QDateTime m_lastSync;
    QList<LogEntry> m_log;
};

#endif // SYNC_SYNCHRONIZER_H_

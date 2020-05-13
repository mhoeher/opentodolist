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

#ifndef SYNC_WEBDAVSYNCHRONIZER_H_
#define SYNC_WEBDAVSYNCHRONIZER_H_

#include <QList>
#include <QLoggingCategory>
#include <QRegularExpression>
#include <QtConcurrent>

#include "sync/synchronizer.h"

class WebDAVClient;

class WebDAVSynchronizer : public Synchronizer
{
    Q_OBJECT
    Q_PROPERTY(QString remoteDirectory READ remoteDirectory WRITE setRemoteDirectory NOTIFY
                       remoteDirectoryChanged)
    Q_PROPERTY(bool disableCertificateCheck READ disableCertificateCheck WRITE
                       setDisableCertificateCheck NOTIFY disableCertificateCheckChanged)
    Q_PROPERTY(QString username READ username WRITE setUsername NOTIFY usernameChanged)
    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)
    Q_PROPERTY(QUrl url READ url WRITE setUrl NOTIFY urlChanged)
    Q_PROPERTY(WebDAVSynchronizer::WebDAVServerType serverType READ serverType WRITE setServerType
                       NOTIFY serverTypeChanged)

#ifdef WEBDAV_SYNCHRONIZER_TEST
    friend class WebDAVSynchronizerTest;
#endif

public:
    enum WebDAVServerType { Unknown = 0, Generic = 1, NextCloud = 2, OwnCloud = 3 };

    Q_ENUM(WebDAVServerType)

    explicit WebDAVSynchronizer(QObject *parent = nullptr);
    ~WebDAVSynchronizer() override;

    QUrl baseUrl() const;

    // Synchronizer interface
    void validate() override;
    void synchronize() override;
    void stopSync() override;
    void findExistingLibraries() override;
    QVariantMap toMap() const override;
    void fromMap(const QVariantMap &map) override;
    void setAccount(Account *account) override;

    QString remoteDirectory() const;
    void setRemoteDirectory(const QString &remoteDirectory);

    bool disableCertificateCheck() const;
    void setDisableCertificateCheck(bool disableCertificateCheck);

    QString username() const;
    void setUsername(const QString &username);

    QString password() const;
    void setPassword(const QString &password);

    QUrl url() const;
    void setUrl(const QUrl &url);

    WebDAVClient *createDAVClient(QObject *parent = nullptr);

    WebDAVServerType serverType() const;
    void setServerType(const WebDAVServerType &serverType);

    bool createDirs() const;
    void setCreateDirs(bool createDirs);

signals:
    void remoteDirectoryChanged();
    void disableCertificateCheckChanged();
    void usernameChanged();
    void passwordChanged();
    void urlChanged();
    void serverTypeChanged();
    void stopRequested();

private:
    static const QString SyncLockFileName;
    static const QString SyncErrorFileName;

    QUrl m_url;
    QString m_remoteDirectory;
    bool m_disableCertificateCheck;
    QString m_username;
    QString m_password;
    bool m_createDirs;
    bool m_stopRequested;
    bool m_hasSyncErrors;
    WebDAVServerType m_serverType;
    QFutureWatcher<QVariantList> m_findExistingEntriesWatcher;

    void touchErrorLock();

    QVariantMap toFullMap() const;
    void fromFullMap(const QVariantMap &map);
};

#endif // SYNC_WEBDAVSYNCHRONIZER_H_

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

#ifndef SYNC_ACCOUNT_H_
#define SYNC_ACCOUNT_H_

#include <QObject>
#include <QUuid>

#include "synchronizer.h"
#include "webdavsynchronizer.h"

class QSettings;

class Account : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Account::Type type READ type WRITE setType NOTIFY typeChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString username READ username WRITE setUsername NOTIFY usernameChanged)
    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)
    Q_PROPERTY(QString baseUrl READ baseUrl WRITE setBaseUrl NOTIFY baseUrlChanged)
    Q_PROPERTY(bool disableCertificateChecks READ disableCertificateChecks WRITE
                       setDisableCertificateChecks NOTIFY disableCertificateChecksChanged)
public:
    explicit Account(QObject* parent = nullptr);

    enum Type { Invalid = 0, WebDAV, NextCloud, OwnCloud };

    Q_ENUM(Type);

    QUuid uid() const;
    void setUid(const QUuid& uid);

    Type type() const;
    void setType(const Type& type);

    QString username() const;
    void setUsername(const QString& username);

    QString password() const;
    void setPassword(const QString& password);

    QString baseUrl() const;
    void setBaseUrl(const QString& baseUrl);

    bool disableCertificateChecks() const;
    void setDisableCertificateChecks(bool disableCertificateChecks);

    void save(QSettings* settings);
    void load(QSettings* settings);

    QString name() const;
    void setName(const QString& name);

    Q_INVOKABLE QVariant toWebDAVServerType() const;
    Q_INVOKABLE Synchronizer* createSynchronizer() const;

signals:

    void uidChanged();
    void typeChanged();
    void nameChanged();
    void usernameChanged();
    void passwordChanged();
    void baseUrlChanged();
    void disableCertificateChecksChanged();

private:
    QUuid m_uid;
    Type m_type;
    QString m_name;
    QString m_username;
    QString m_password;
    QString m_baseUrl;
    bool m_disableCertificateChecks;
};

#endif // SYNC_ACCOUNT_H_

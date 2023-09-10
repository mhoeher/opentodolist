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

#ifndef SYNC_WEBDAVACCOUNT_H_
#define SYNC_WEBDAVACCOUNT_H_

#include <QObject>
#include <QString>
#include <QVariantMap>

#include "account.h"

namespace SynqClient {
// Forward declarations
class WebDAVJobFactory;
enum class WebDAVServerType : quint32;
}

class WebDAVAccount : public Account
{
    Q_OBJECT
    Q_PROPERTY(QString username READ username WRITE setUsername NOTIFY usernameChanged)
    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)
    Q_PROPERTY(QString baseUrl READ baseUrl WRITE setBaseUrl NOTIFY baseUrlChanged)
    Q_PROPERTY(bool disableCertificateChecks READ disableCertificateChecks WRITE
                       setDisableCertificateChecks NOTIFY disableCertificateChecksChanged)
    Q_PROPERTY(QVariantMap backendSpecificData READ backendSpecificData WRITE setBackendSpecificData
                       NOTIFY backendSpecificDataChanged)
public:
    explicit WebDAVAccount(QObject* parent = nullptr);

    QString username() const;
    void setUsername(const QString& username);

    QString password() const;
    void setPassword(const QString& password);

    QString baseUrl() const;
    void setBaseUrl(const QString& baseUrl);

    bool disableCertificateChecks() const;
    void setDisableCertificateChecks(bool disableCertificateChecks);

    const QVariantMap& backendSpecificData() const;
    void setBackendSpecificData(const QVariantMap& newBackendSpecificData);

signals:

    void usernameChanged();
    void passwordChanged();
    void baseUrlChanged();
    void disableCertificateChecksChanged();

    void backendSpecificDataChanged();

private:
    QString m_username;
    QString m_password;
    QString m_baseUrl;
    bool m_disableCertificateChecks;
    QVariantMap m_backendSpecificData;

    SynqClient::WebDAVJobFactory* createWebDAVJobFactory(QObject* parent = nullptr);

    // Account interface
public:
    void save(QSettings* settings) override;
    void load(QSettings* settings) override;
    int preferredSyncIntervalInSec() const override;
    QString accountSecrets() const override;
    void setAccountSecrets(const QString& secrets) override;
    Synchronizer* createSynchronizer() const override;
    void login() override;
    void findExistingLibraries() override;
    void checkConnectivity() override;

protected:
    virtual void fillServerType(SynqClient::WebDAVServerType& type) const;
};

#endif // SYNC_WEBDAVACCOUNT_H_

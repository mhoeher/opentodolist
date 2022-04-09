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

#include "webdavaccount.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>

#include <SynqClient/WebDAVJobFactory>
#include <SynqClient/libsynqclient.h>

/**
 * @brief An account used to connect to a WebDAV server.
 *
 * This class implements an account class used to connect to WebDAV servers.
 */

/**
 * @brief Constructor.
 */
WebDAVAccount::WebDAVAccount(QObject* parent)
    : Account { parent },
      m_username(),
      m_password(),
      m_baseUrl(),
      m_disableCertificateChecks(false),
      m_backendSpecificData()
{
    setType(WebDAV);
}

/**
 * @brief The username to use to log in.
 */
QString WebDAVAccount::username() const
{
    return m_username;
}

/**
 * @brief Set the @p username to use to log in.
 */
void WebDAVAccount::setUsername(const QString& username)
{
    if (m_username != username) {
        m_username = username;
        emit usernameChanged();
    }
}

/**
 * @brief The password used to log in to the server.
 */
QString WebDAVAccount::password() const
{
    return m_password;
}

/**
 * @brief Set the password used to log in to the password.
 */
void WebDAVAccount::setPassword(const QString& password)
{
    if (m_password != password) {
        m_password = password;
        emit passwordChanged();
    }
}

/**
 * @brief The base URL used to connect to the server.
 *
 * This is the URL without the protocol part of the server to connect to.
 */
QString WebDAVAccount::baseUrl() const
{
    return m_baseUrl;
}

/**
 * @brief Set the @p baseUrl of the server to connect to.
 */
void WebDAVAccount::setBaseUrl(const QString& baseUrl)
{
    if (m_baseUrl != baseUrl) {
        m_baseUrl = baseUrl;
        emit baseUrlChanged();
    }
}

/**
 * @brief Shall certificate checks be skipped.
 *
 * If this property is false (the default) and secure connections to the
 * server are used, check the HTTPS server certificates. If this property is
 * true, these checks will be skiped. This can be used when connecting to
 * a server which uses self signed certificates.
 */
bool WebDAVAccount::disableCertificateChecks() const
{
    return m_disableCertificateChecks;
}

/**
 * @brief Set if certificate checks shall be skiped.
 */
void WebDAVAccount::setDisableCertificateChecks(bool disableCertificateChecks)
{
    if (m_disableCertificateChecks != disableCertificateChecks) {
        m_disableCertificateChecks = disableCertificateChecks;
        emit disableCertificateChecksChanged();
    }
}

/**
 * @brief Data specific to the used backend.
 */
const QVariantMap& WebDAVAccount::backendSpecificData() const
{
    return m_backendSpecificData;
}

void WebDAVAccount::setBackendSpecificData(const QVariantMap& newBackendSpecificData)
{
    if (m_backendSpecificData == newBackendSpecificData)
        return;
    m_backendSpecificData = newBackendSpecificData;
    emit backendSpecificDataChanged();
}

SynqClient::WebDAVJobFactory* WebDAVAccount::createWebDAVJobFactory(QObject* parent)
{
    auto factory = new SynqClient::WebDAVJobFactory(parent);

    auto nam = new QNetworkAccessManager(factory);
    nam->setRedirectPolicy(QNetworkRequest::NoLessSafeRedirectPolicy);
    connect(nam, &QNetworkAccessManager::sslErrors, this,
            [=](QNetworkReply* reply, const QList<QSslError>& errors) {
                if (!m_disableCertificateChecks) {
                    reply->ignoreSslErrors(errors);
                }
            });

    factory->setNetworkAccessManager(nam);
    SynqClient::WebDAVServerType serverType;
    fillServerType(serverType);
    factory->setServerType(serverType);
    auto url = QUrl(m_baseUrl);
    url.setUserName(m_username);
    url.setPassword(m_password);
    factory->setUrl(url);
    factory->setUserAgent(Synchronizer::HTTPUserAgent);
    factory->setWorkarounds(static_cast<SynqClient::WebDAVWorkarounds>(
            m_backendSpecificData.value("workarounds", 0).toInt()));

    return factory;
}

void WebDAVAccount::save(QSettings* settings)
{
    Account::save(settings);
    settings->setValue("username", m_username);
    settings->setValue("baseUrl", m_baseUrl);
    settings->setValue("disableCertificateChecks", m_disableCertificateChecks);
    settings->setValue("backendSpecificData", m_backendSpecificData);
}

void WebDAVAccount::load(QSettings* settings)
{
    Account::load(settings);
    m_username = settings->value("username", m_username).toString();
    m_baseUrl = settings->value("baseUrl", m_baseUrl).toString();
    m_disableCertificateChecks =
            settings->value("disableCertificateChecks", m_disableCertificateChecks).toBool();
    m_backendSpecificData = settings->value("backendSpecificData", m_backendSpecificData).toMap();
}

QString WebDAVAccount::accountSecrets() const
{
    return m_password;
}

void WebDAVAccount::setAccountSecrets(const QString& secrets)
{
    setPassword(secrets);
}

Synchronizer* WebDAVAccount::createSynchronizer() const
{
    auto sync = new WebDAVSynchronizer;
    sync->setUsername(m_username);
    sync->setPassword(m_password);
    sync->setUrl(m_baseUrl);
    sync->setServerType(WebDAVSynchronizer::Generic);
    return sync;
}

void WebDAVAccount::login()
{
    setOnline(false);
    setLoggingIn(true);

    auto factory = createWebDAVJobFactory(this);
    factory->setWorkarounds(SynqClient::WebDAVWorkaround::NoWorkarounds); // clear workarounds
    connect(factory, &SynqClient::WebDAVJobFactory::serverTestFinished, this, [=](bool success) {
        factory->deleteLater();
        if (success) {
            m_backendSpecificData["workarounds"] = static_cast<int>(factory->workarounds());
            setOnline(true);
        }
        setLoggingIn(false);
        emit loginFinished(success);
    });
    factory->testServer();
}

void WebDAVAccount::fillServerType(SynqClient::WebDAVServerType& type) const
{
    type = SynqClient::WebDAVServerType::Generic;
}

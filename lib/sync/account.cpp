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

#include <QSettings>

#include "account.h"

/**
 * @class Account
 * @brief Represents a connection to a server.
 *
 * The account class represents the connection to a server. First of all,
 * it consists of a type, which determines the type of server to talk to.
 * Depending on the concrete type, other properties of the class are relevant,
 * like a server host name, user name and password and so on.
 *
 * Connections to servers are required to synchronize libraries: The app will
 * push data to and pull data from a server in order to ensure user data is
 * backed up and can be accessed also on other devices.
 *
 * Note that one account can be associated with more than one library on
 * a device.
 */

/**
 * @brief Constructor.
 */
Account::Account(QObject* parent)
    : QObject(parent),
      m_uid(QUuid::createUuid()),
      m_type(Invalid),
      m_username(),
      m_password(),
      m_baseUrl(),
      m_disableCertificateChecks(false),
      m_backendSpecificData()
{
}

/**
 * @brief The type of server to connect to.
 *
 * This property determines the type of server the account represents
 * a connection to.
 */
Account::Type Account::type() const
{
    return m_type;
}

/**
 * @brief Set the account @p type.
 */
void Account::setType(const Type& type)
{
    if (m_type != type) {
        m_type = type;
        emit typeChanged();
    }
}

/**
 * @brief The username to use to log in.
 */
QString Account::username() const
{
    return m_username;
}

/**
 * @brief Set the @p username to use to log in.
 */
void Account::setUsername(const QString& username)
{
    if (m_username != username) {
        m_username = username;
        emit usernameChanged();
    }
}

/**
 * @brief The password used to log in to the server.
 */
QString Account::password() const
{
    return m_password;
}

/**
 * @brief Set the password used to log in to the password.
 */
void Account::setPassword(const QString& password)
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
QString Account::baseUrl() const
{
    return m_baseUrl;
}

/**
 * @brief Set the @p baseUrl of the server to connect to.
 */
void Account::setBaseUrl(const QString& baseUrl)
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
bool Account::disableCertificateChecks() const
{
    return m_disableCertificateChecks;
}

/**
 * @brief Set if certificate checks shall be skiped.
 */
void Account::setDisableCertificateChecks(bool disableCertificateChecks)
{
    if (m_disableCertificateChecks != disableCertificateChecks) {
        m_disableCertificateChecks = disableCertificateChecks;
        emit disableCertificateChecksChanged();
    }
}

/**
 * @brief Save the account to the @p settings.
 */
void Account::save(QSettings* settings)
{
    q_check_ptr(settings);
    settings->setValue("type", QVariant::fromValue(m_type).toString());
    settings->setValue("username", m_username);
    settings->setValue("name", m_name);
    settings->setValue("baseUrl", m_baseUrl);
    settings->setValue("disableCertificateChecks", m_disableCertificateChecks);
    settings->setValue("backendSpecificData", m_backendSpecificData);
}

/**
 * @brief Restore the account from the @p settings.
 */
void Account::load(QSettings* settings)
{
    q_check_ptr(settings);
    m_type = settings->value("type", QVariant::fromValue(m_type).toString()).value<Type>();
    m_username = settings->value("username", m_username).toString();
    m_name = settings->value("name", m_name).toString();
    m_baseUrl = settings->value("baseUrl", m_baseUrl).toString();
    m_disableCertificateChecks =
            settings->value("disableCertificateChecks", m_disableCertificateChecks).toBool();
    m_backendSpecificData = settings->value("backendSpecificData", m_backendSpecificData).toMap();
}

/**
 * @brief The name of the account.
 *
 * This is the human readable name of the account. It is used inside
 * the application to identify the account.
 */
QString Account::name() const
{
    return m_name;
}

/**
 * @brief Set the name of the account.
 */
void Account::setName(const QString& name)
{
    if (m_name != name) {
        m_name = name;
        emit nameChanged();
    }
}

/**
 * @brief Convert the account type to WebDAV server type.
 *
 * If the account represents a connection to a WebDAV like service,
 * return the appropriate WebDAV synchronizer type.
 *
 * For other accounts, this returns the `Unknown` server type.
 */
QVariant Account::toWebDAVServerType() const
{
    switch (m_type) {
    case NextCloud:
        return WebDAVSynchronizer::NextCloud;
    case OwnCloud:
        return WebDAVSynchronizer::OwnCloud;
    case WebDAV:
        return WebDAVSynchronizer::Generic;
    case Invalid:
        return WebDAVSynchronizer::Unknown;
    }
    return QVariant();
}

/**
 * @brief Create a synchronizer from the account.
 *
 * This creates a synchronizer from the account which can be used for querying the remote server.
 *
 * @note The caller takes ownership of the create object.
 */
Synchronizer* Account::createSynchronizer() const
{
    switch (m_type) {
    case NextCloud: {
        auto sync = new WebDAVSynchronizer;
        sync->setUsername(m_username);
        sync->setPassword(m_password);
        sync->setUrl(m_baseUrl);
        sync->setServerType(WebDAVSynchronizer::NextCloud);
        return sync;
    }
    case OwnCloud: {
        auto sync = new WebDAVSynchronizer;
        sync->setUsername(m_username);
        sync->setPassword(m_password);
        sync->setUrl(m_baseUrl);
        sync->setServerType(WebDAVSynchronizer::OwnCloud);
        return sync;
    }
    case WebDAV: {
        auto sync = new WebDAVSynchronizer;
        sync->setUsername(m_username);
        sync->setPassword(m_password);
        sync->setUrl(m_baseUrl);
        sync->setServerType(WebDAVSynchronizer::Generic);
        return sync;
    }
    case Invalid:
        return nullptr;
    }
    return nullptr;
}

/**
 * @brief Data specific to the used backend.
 */
const QVariantMap& Account::backendSpecificData() const
{
    return m_backendSpecificData;
}

void Account::setBackendSpecificData(const QVariantMap& newBackendSpecificData)
{
    if (m_backendSpecificData == newBackendSpecificData)
        return;
    m_backendSpecificData = newBackendSpecificData;
    emit backendSpecificDataChanged();
}

/**
 * @brief The globally unique ID of the account.
 */
QUuid Account::uid() const
{
    return m_uid;
}

/**
 * @brief Set the globally unique ID of the account.
 */
void Account::setUid(const QUuid& uid)
{
    if (m_uid != uid) {
        m_uid = uid;
        emit uidChanged();
    }
}

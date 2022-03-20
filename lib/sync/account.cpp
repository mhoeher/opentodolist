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

#include <QMetaEnum>
#include <QSettings>

#include "account.h"
#include "webdavaccount.h"
#include "nextcloudaccount.h"
#include "owncloudaccount.h"

/**
 * @class Account
 * @brief Represents a connection to a server.
 *
 * The account class represents the connection to a server. First of all,
 * it consists of a type, which determines the type of server to talk to.
 * This class itself is never instantiated - instead, depending of the type,
 * one of the concrete account types will be used instead.
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
Account::Account(QObject* parent) : QObject(parent), m_uid(QUuid::createUuid()), m_type(Invalid) {}

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
 * @brief Create an account object.
 *
 * This factory method is used to create an account according to the requested account
 * @p type.
 *
 * @note The caller gets ownership of the returned object.
 */
Account* Account::createAccount(Type type, QObject* parent)
{
    switch (type) {
    case WebDAV:
        return new WebDAVAccount(parent);
    case NextCloud:
        return new NextCloudAccount(parent);
    case OwnCloud:
        return new OwnCloudAccount(parent);
    case Invalid:
        return nullptr;
    }
}

/**
 * @brief Create an account object.
 *
 * This is an overloaded version of the factory method. It takes an @p settings object which
 * currently has an active group corresponding to an account to be read from settings. This method
 * will read the account type from the settings and construct an appropriate settings object from
 * it.
 *
 * @note The ownership of the resulting object goes to the caller. This method will not call
 * Account::load() - this is up to the caller as well.
 */
Account* Account::createAccount(QSettings* settings, QObject* parent)
{
    q_check_ptr(settings);
    auto type = settings->value("type", QVariant::fromValue(Invalid).toString()).value<Type>();
    return createAccount(type, parent);
}

/**
 * @brief Convert the account @p type to a string.
 */
QString Account::typeToString(Type type)
{
    auto metaEnum = QMetaEnum::fromType<Type>();
    return metaEnum.valueToKey(type);
}

/**
 * @brief Save the account to the @p settings.
 *
 * Concrete sub-classes shall extend this to include saving (non-sensible) information in the
 * @p settings.
 */
void Account::save(QSettings* settings)
{
    q_check_ptr(settings);
    settings->setValue("type", QVariant::fromValue(m_type).toString());
    settings->setValue("name", m_name);
}

/**
 * @brief Restore the account from the @p settings.
 *
 * Concrete sub-classes shall extend this to read back their own (non-sensible) information
 * from the @p settings.
 */
void Account::load(QSettings* settings)
{
    q_check_ptr(settings);
    m_type = settings->value("type", QVariant::fromValue(m_type).toString()).value<Type>();
    m_name = settings->value("name", m_name).toString();
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
 * @brief Set the type of the account.
 *
 * This shall be used in concrete sub-classes to set the concrete type of account.
 */
void Account::setType(const Type& type)
{
    m_type = type;
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

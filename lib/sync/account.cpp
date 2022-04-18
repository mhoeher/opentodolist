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
Account::Account(QObject* parent)
    : QObject(parent),
      m_uid(QUuid::createUuid()),
      m_type(Invalid),
      m_name(),
      m_loggingIn(false),
      m_online(false),
      m_findingRemoteLibraries(false),
      m_remoteLibraries()
{
    qRegisterMetaType<QList<RemoteLibraryInfo>>();
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
    return nullptr; // To silence cppcheck
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
 * @brief Set the type of the account.
 *
 * This shall be used in concrete sub-classes to set the concrete type of account.
 */
void Account::setType(const Type& type)
{
    m_type = type;
}

/**
 * @brief Indicated that currently a login procedure is running.
 *
 * This property indicates if currently a login is running. A login is typically started by calling
 * login(). The account will then go into the "logging in" state.
 */
bool Account::loggingIn() const
{
    return m_loggingIn;
}

/**
 * @brief Set the loggingIn property.
 */
void Account::setLoggingIn(bool newLoggingIn)
{
    if (m_loggingIn == newLoggingIn)
        return;
    m_loggingIn = newLoggingIn;
    emit loggingInChanged();
}

/**
 * @brief Indicates if the account is online.
 *
 * This checks if the account is online. Being online indicates that the account is set up and ready
 * to communicate with the remote server.
 */
bool Account::online() const
{
    return m_online;
}

/**
 * @brief Set the online attribute.
 */
void Account::setOnline(bool newOnline)
{
    if (m_online == newOnline)
        return;
    m_online = newOnline;
    emit onlineChanged();
}

/**
 * @brief The list of remote libraries stored in the account.
 *
 * This is the list of accounts that are stored remotely on the account. Not all acounts might also
 * be synchronized locally. To populate this list, call findExistingLibraries().
 */
const QList<RemoteLibraryInfo>& Account::remoteLibraries() const
{
    return m_remoteLibraries;
}

/**
 * @brief Set the list of existing remote libraries.
 */
void Account::setRemoteLibraries(const QList<RemoteLibraryInfo>& newRemoteLibraries)
{
    if (m_remoteLibraries == newRemoteLibraries)
        return;
    m_remoteLibraries = newRemoteLibraries;
    emit remoteLibrariesChanged();
}

/**
 * @brief The account is currently looking for existing libraries on the server.
 *
 * If this property is true, the account is currently looking for existing libraries on the remote.
 */
bool Account::findingRemoteLibraries() const
{
    return m_findingRemoteLibraries;
}

/**
 * @brief Set the findingRemoteLibraries property.
 */
void Account::setFindingRemoteLibraries(bool newFindingRemoteLibraries)
{
    if (m_findingRemoteLibraries == newFindingRemoteLibraries)
        return;
    m_findingRemoteLibraries = newFindingRemoteLibraries;
    emit findingRemoteLibrariesChanged();
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

/**
 * @brief Log into the account.
 *
 * Logging in to the account checks if - given the setings of the account - we can communicate with
 * the remote server.
 *
 * The default implementation does nothing but emit
 * the loginFinished() signal with an error value and set the online property to false. Concrete
 * subclasses shall implement this method and handle the login procedure accordingly. In particular:
 *
 * - Set the loggingIn property to true.
 * - Set the online property to false.
 * - Do whatever is needed to log in and bring the account online.
 * - Set the online property accordingly to the result of the procedure.
 * - Set the loggingIn property to false.
 * - Emit the loginFinished() signal with a value indicating if the login was successful.
 */
void Account::login()
{
    setOnline(false);
    emit loginFinished(false);
}

/**
 * @brief Start searching for existing libraries.
 *
 * This method triggers a search for existing libraries.
 *
 * When the search is finished,
 * the existingLibraries property of the synchronizer is updated.
 *
 * The default implementation of this method does nothing. Sub-classes
 * can implement it if the appropriate functionality is supported by
 * the respective backends. The implementation should:
 *
 * 1. Call setFindingRemoteLibraries(true).
 * 2. Look for existing libraries and populate the remoteLibraries property.
 * 3. Call setFindingRemoteLibraries(false).
 */
void Account::findExistingLibraries() {}

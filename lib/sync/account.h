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
#include "remotelibraryinfo.h"

class QSettings;
class ApplicationSettings;

class Account : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QUuid uid READ uid NOTIFY uidChanged)
    Q_PROPERTY(Account::Type type READ type NOTIFY typeChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(bool loggingIn READ loggingIn NOTIFY loggingInChanged)
    Q_PROPERTY(bool online READ online WRITE setOnline NOTIFY onlineChanged)
    Q_PROPERTY(QList<RemoteLibraryInfo> remoteLibraries READ remoteLibraries WRITE
                       setRemoteLibraries NOTIFY remoteLibrariesChanged)
    Q_PROPERTY(bool findingRemoteLibraries READ findingRemoteLibraries WRITE
                       setFindingRemoteLibraries NOTIFY findingRemoteLibrariesChanged)

    friend class ApplicationSettings;

public:
    explicit Account(QObject* parent = nullptr);

    enum Type { Invalid = 0, WebDAV, NextCloud, OwnCloud, Dropbox };

    Q_ENUM(Type);

    QUuid uid() const;
    void setUid(const QUuid& uid);

    Type type() const;

    static Account* createAccount(Type type, QObject* parent = nullptr);
    static Account* createAccount(QSettings* settings, QObject* parent = nullptr);
    static QString typeToString(Type type);

    virtual void save(QSettings* settings);
    virtual void load(QSettings* settings);

    Q_INVOKABLE virtual void login();
    Q_INVOKABLE virtual void findExistingLibraries();
    Q_INVOKABLE virtual void checkConnectivity();

    /**
     * @brief Get secrets (e.g. passwords) needed for communication with the server.
     */
    virtual QString accountSecrets() const = 0;

    /**
     * @brief Set the account secrets.
     */
    virtual void setAccountSecrets(const QString& secrets) = 0;

    QString name() const;
    void setName(const QString& name);

    /**
     * @brief Create a Synchronizer suitable for running a sync against that account.
     *
     *  @note The caller takes ownership of the create object.
     */
    Q_INVOKABLE virtual Synchronizer* createSynchronizer() const = 0;

    bool loggingIn() const;

    bool online() const;

    const QList<RemoteLibraryInfo>& remoteLibraries() const;

    bool findingRemoteLibraries() const;

signals:

    void uidChanged();
    void typeChanged();
    void nameChanged();
    void loggingInChanged();
    void onlineChanged();

    /**
     * @brief Indicates that the login procedure finished.
     *
     * This signal is emitted to indicate that the login procedure has finished. The @p success
     * parameter indicated if the login was successfull or not.
     *
     * If an error occurred, the loginError() signal might have been emitted alongside this signal.
     */
    void loginFinished(bool success);

    /**
     * @brief Indicates that there was an error logging in.
     *
     * This signal is emitted during a login if an error occurs. It carries a descriptive @p message
     * which should be shown to the user.
     */
    void loginError(const QString& message);

    /**
     * @brief The list of remote libraries changed.
     */
    void remoteLibrariesChanged();

    /**
     * @brief The findingRemoteLibraries property has changed.
     */
    void findingRemoteLibrariesChanged();

    /**
     * @brief The connectivity check finished.
     *
     * This signal is emitted to indicate that the check for connectivity to the server has
     * finished. The @p online argument indicates if the account currently is online, i.e. the
     * check was successful. If it is false, it means that no connection to the account could
     * be made.
     */
    void connectivityCheckFinished(bool online);

    /**
     * @brief Indicates that the account secrets have changed.
     */
    void accountSecretsChanged();

protected:
    void setType(const Type& type);
    void setLoggingIn(bool newLoggingIn);
    void setOnline(bool newOnline);
    void setRemoteLibraries(const QList<RemoteLibraryInfo>& newRemoteLibraries);
    void setFindingRemoteLibraries(bool newFindingRemoteLibraries);

private:
    QUuid m_uid;
    Type m_type;
    QString m_name;
    bool m_loggingIn;
    bool m_online;
    bool m_findingRemoteLibraries;
    QList<RemoteLibraryInfo> m_remoteLibraries;
};

#endif // SYNC_ACCOUNT_H_

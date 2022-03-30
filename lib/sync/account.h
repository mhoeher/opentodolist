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
class ApplicationSettings;

class Account : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Account::Type type READ type NOTIFY typeChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(bool loggingIn READ loggingIn NOTIFY loggingInChanged)

    friend class ApplicationSettings;

public:
    explicit Account(QObject* parent = nullptr);

    enum Type { Invalid = 0, WebDAV, NextCloud, OwnCloud };

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

protected:
    void setType(const Type& type);
    void setLoggingIn(bool newLoggingIn);
    void setOnline(bool newOnline);

private:
    QUuid m_uid;
    Type m_type;
    QString m_name;
    bool m_loggingIn;
    bool m_online;

    Q_PROPERTY(bool online READ online WRITE setOnline NOTIFY onlineChanged)
};

#endif // SYNC_ACCOUNT_H_

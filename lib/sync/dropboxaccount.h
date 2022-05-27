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

#ifndef SYNC_DROPBOXACCOUNT_H_
#define SYNC_DROPBOXACCOUNT_H_

#include <QObject>

#include "account.h"

// Forward declaractions:
class QOAuth2AuthorizationCodeFlow;
namespace SynqClient {
class DropboxJobFactory;
}

class DropboxAccount : public Account
{
    Q_OBJECT

    Q_PROPERTY(QString accessToken READ accessToken WRITE setAccessToken NOTIFY accessTokenChanged)

public:
    explicit DropboxAccount(QObject* parent = nullptr);

    const QString& accessToken() const;

public slots:

    void verifyTokens();

signals:

    void accessTokenChanged();
    void authorizationUrlReceived(const QUrl& url);

private:
    // OAuth props
    QString m_refreshToken;
    QString m_accessToken;
    QDateTime m_expiration;

    // PKCE extension:
    QString m_codeVerifier;

    void setAccessToken(const QString& newAccessToken);

    // Utility methods
    static QString createCodeVerifier();
    QString getCodeChallenge() const;
    SynqClient::DropboxJobFactory* createDropboxJobFactory(QObject* parent = nullptr) const;
    QOAuth2AuthorizationCodeFlow* createOAuthAuthFlow(QObject* parent = nullptr) const;

    // Account interface
public:
    void save(QSettings* settings) override;
    void load(QSettings* settings) override;
    QString accountSecrets() const override;
    void setAccountSecrets(const QString& secrets) override;
    Synchronizer* createSynchronizer() const override;
    void login() override;
    void findExistingLibraries() override;
    void checkConnectivity() override;
};

#endif // SYNC_DROPBOXACCOUNT_H_

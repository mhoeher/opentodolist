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

#include "dropboxaccount.h"

#include <QVariant>
#include <QVariantMap>
#include <QJsonDocument>

#include "dropboxsynchronizer.h"

static const char* RefreshToken = "refreshToken";
static const char* AccessToken = "accessToken";
static const char* CodeVerifier = "codeVerifier";
static const char* Expiration = "expiration";

DropboxAccount::DropboxAccount(QObject* parent)
    : Account { parent }, m_refreshToken(), m_accessToken(), m_codeVerifier()
{
}

const QString& DropboxAccount::accessToken() const
{
    return m_accessToken;
}

void DropboxAccount::setAccessToken(const QString& newAccessToken)
{
    if (m_accessToken == newAccessToken)
        return;
    m_accessToken = newAccessToken;
    emit accessTokenChanged();
}

void DropboxAccount::save(QSettings* settings)
{
    Account::save(settings);
    settings->setValue(Expiration, m_expiration);
}

void DropboxAccount::load(QSettings* settings)
{
    Account::load(settings);
    m_expiration = settings->value(Expiration, m_expiration).toDateTime();
}

QString DropboxAccount::accountSecrets() const
{
    QVariantMap doc {
        { RefreshToken, m_refreshToken },
        { AccessToken, m_accessToken },
        { CodeVerifier, m_codeVerifier },
    };
    return QJsonDocument::fromVariant(doc).toJson();
}

void DropboxAccount::setAccountSecrets(const QString& secrets)
{
    auto doc = QJsonDocument::fromJson(secrets.toUtf8()).toVariant().toMap();
    m_refreshToken = doc.value(RefreshToken, m_refreshToken).toString();
    m_accessToken = doc.value(AccessToken, m_accessToken).toString();
    m_codeVerifier = doc.value(CodeVerifier, m_codeVerifier).toString();
}

Synchronizer* DropboxAccount::createSynchronizer() const
{
    auto result = new DropboxSynchronizer();
    result->setAccessToken(m_accessToken);
    return result;
}

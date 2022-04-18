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

#include "dropboxsynchronizer.h"

#include "dropboxaccount.h"

DropboxSynchronizer::DropboxSynchronizer(QObject* parent) : Synchronizer(parent), m_accessToken() {}

void DropboxSynchronizer::synchronize() {}

void DropboxSynchronizer::stopSync() {}

QVariantMap DropboxSynchronizer::toMap() const
{
    return QVariantMap {};
}

void DropboxSynchronizer::fromMap(const QVariantMap& map) {}

void DropboxSynchronizer::setAccount(Account* account)
{
    auto dropboxAccount = qobject_cast<DropboxAccount*>(account);
    q_check_ptr(dropboxAccount);

    setAccessToken(dropboxAccount->accessToken());
}

const QString& DropboxSynchronizer::accessToken() const
{
    return m_accessToken;
}

void DropboxSynchronizer::setAccessToken(const QString& newAccessToken)
{
    if (m_accessToken == newAccessToken)
        return;
    m_accessToken = newAccessToken;
    emit accessTokenChanged();
}

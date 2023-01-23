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

#include "nextcloudaccount.h"

#include <SynqClient/libsynqclient.h>

#include "sync/webdavsynchronizer.h"

NextCloudAccount::NextCloudAccount(QObject* parent) : WebDAVAccount { parent }
{
    setType(NextCloud);
}

Synchronizer* NextCloudAccount::createSynchronizer() const
{
    auto result = WebDAVAccount::createSynchronizer();
    auto webDAVSync = qobject_cast<WebDAVSynchronizer*>(result);
    Q_ASSERT(webDAVSync);
    webDAVSync->setServerType(WebDAVSynchronizer::NextCloud);
    return result;
}

void NextCloudAccount::fillServerType(SynqClient::WebDAVServerType& type) const
{
    type = SynqClient::WebDAVServerType::NextCloud;
}

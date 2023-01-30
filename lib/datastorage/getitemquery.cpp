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

#include <qlmdb/database.h>
#include <qlmdb/transaction.h>

#include "datamodel/item.h"
#include "datastorage/getitemquery.h"

GetItemQuery::GetItemQuery(QObject* parent) : ItemsQuery(parent), m_uid() {}

QUuid GetItemQuery::uid() const
{
    return m_uid;
}

void GetItemQuery::setUid(const QUuid& uid)
{
    m_uid = uid;
}

void GetItemQuery::run()
{
    QLMDB::Transaction t(*context(), QLMDB::Transaction::ReadOnly);
    auto uid = m_uid.toByteArray();
    auto data = items()->get(t, uid);
    if (!data.isNull()) {
        auto entry = ItemCacheEntry::fromByteArray(data, uid);
        if (entry.valid) {
            calculateValues(t, &entry);
            emit itemLoaded(QVariant::fromValue(entry));
        }
    }
}

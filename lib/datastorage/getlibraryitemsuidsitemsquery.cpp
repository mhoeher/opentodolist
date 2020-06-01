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

#include "datamodel/library.h"
#include "datastorage/getlibraryitemsuidsitemsquery.h"

GetLibraryItemsUIDsItemsQuery::GetLibraryItemsUIDsItemsQuery(QObject* parent)
    : ItemsQuery(parent), m_itemQueue()
{
    qRegisterMetaType<QSet<QUuid>>();
}

/**
 * @brief Add a library to the query.
 *
 * This will cause the query to retrieve all UIDs of items in the
 * @p library. The retrieval is done recursively.
 */
void GetLibraryItemsUIDsItemsQuery::addLibrary(const Library* library)
{
    if (library != nullptr) {
        addLibrary(library->uid());
    }
}

/**
 * @brief Add a library to the query.
 *
 * This is an overloaded version of the addLibrary, which takes the UID of a
 * library to retrieve item UIDs for.
 */
void GetLibraryItemsUIDsItemsQuery::addLibrary(const QUuid uid)
{
    if (!uid.isNull()) {
        m_itemQueue << uid;
    }
}

void GetLibraryItemsUIDsItemsQuery::run()
{
    QSet<QUuid> result;
    QLMDB::Transaction t(*context());

    while (!m_itemQueue.isEmpty()) {
        auto nextId = m_itemQueue.dequeue();
        auto children = this->children()->getAll(t, nextId.toByteArray());
        for (auto& child : children) {
            auto cid = QUuid(child);
            if (!result.contains(cid)) {
                result << cid;
                m_itemQueue << cid;
            }
        }
    }

    emit uidsAvailable(result);
}

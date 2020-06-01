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

#include <QQueue>

#include <qlmdb/transaction.h>
#include <qlmdb/database.h>

#include "datamodel/item.h"
#include "datamodel/library.h"
#include "datastorage/deleteitemsquery.h"

DeleteItemsQuery::DeleteItemsQuery(QObject* parent) : ItemsQuery(parent), m_itemsToDelete() {}

void DeleteItemsQuery::deleteItem(const Item* item)
{
    if (item != nullptr) {
        deleteItem(item->uid());
    }
}

void DeleteItemsQuery::deleteItem(const QUuid& uid)
{
    if (!uid.isNull()) {
        m_itemsToDelete << ItemToDelete({ uid, false, false });
    }
}

void DeleteItemsQuery::deleteLibrary(const Library* library, bool deleteLibraryDir)
{
    if (library != nullptr) {
        deleteLibrary(library->uid(), deleteLibraryDir);
    }
}

void DeleteItemsQuery::deleteLibrary(const QUuid& uid, bool deleteLibraryDir)
{
    if (!uid.isNull()) {
        m_itemsToDelete << ItemToDelete({ uid, true, deleteLibraryDir });
    }
}

void DeleteItemsQuery::run()
{
    QLMDB::Transaction t(*context());
    for (auto itemToDelete : m_itemsToDelete) {
        if (!itemToDelete.uid.isNull()) {
            QQueue<QByteArray> queue;
            queue << itemToDelete.uid.toByteArray(); // NOLINT

            // If the item is a library and we are requested to delete the
            // library directory, do it now:
            if (itemToDelete.isLibrary && itemToDelete.deleteLibraryDir) {
                auto libData = items()->get(t, itemToDelete.uid.toByteArray());
                if (!libData.isEmpty()) {
                    auto lib = Library::decache(LibraryCacheEntry::fromByteArray(
                            libData, itemToDelete.uid.toByteArray()));
                    if (lib) {
                        lib->deleteLibrary();
                        delete lib;
                    }
                }
            }

            while (!queue.isEmpty()) {
                auto nextId = queue.dequeue();
                if (!itemToDelete.isLibrary) {
                    // When we delete single items, we have to do it recursively
                    // and on disk.
                    // Otherwise, orphaned item files will remain in the library
                    // directory and added back on every load, however, they will
                    // be unreachable and hence spam our cache.
                    auto itemData = items()->get(t, nextId);
                    if (!itemData.isEmpty()) {
                        auto item = Item::decache(ItemCacheEntry::fromByteArray(itemData, nextId));
                        if (item != nullptr) {
                            item->deleteItem();
                            delete item;
                        }
                        markAsChanged(&t, item->uid().toByteArray());
                    }
                } else {
                    if (!items()->get(t, nextId).isNull()) {
                        markAsChanged(&t, nextId);
                    }
                }
                items()->remove(t, nextId);
                auto keys = children()->getAll(nextId);
                queue.append(keys);
                children()->remove(t, nextId);
            }
        }
        emit itemDeleted();
    }
}

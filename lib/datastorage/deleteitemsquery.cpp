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
        m_itemsToDelete << ItemToDelete::item(uid);
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
        m_itemsToDelete << ItemToDelete::library(uid, deleteLibraryDir);
    }
}

/**
 * @brief Delete the children of the @p item.
 *
 * This causes the children of the item to be deleted, but not the item itself.
 * If @p childFilter is not null, it will be called on each child item and only children
 * for which it returns true will be deleted.
 */
void DeleteItemsQuery::deleteChildrenOfItem(const Item* item,
                                            DeleteItemsQuery::ItemFilter childFilter)
{
    if (item != nullptr) {
        deleteChildrenOfItem(item->uid(), childFilter);
    }
}

/**
 * @brief Delete the children of the item with the given @p uid.
 *
 * This is an overloaded method which takes the uid of an item instead of the item itself.
 */
void DeleteItemsQuery::deleteChildrenOfItem(const QUuid& uid,
                                            DeleteItemsQuery::ItemFilter childFilter)
{
    if (!uid.isNull()) {
        m_itemsToDelete << ItemToDelete::children(uid, childFilter);
    }
}

void DeleteItemsQuery::deleteLibraryFromDisk(const ItemToDelete& itemToDelete,
                                             QLMDB::Transaction& t)
{
    if (itemToDelete.isLibrary && itemToDelete.deleteLibraryDir) {
        auto libData = items()->get(t, itemToDelete.uid.toByteArray());
        if (!libData.isEmpty()) {
            auto lib = Library::decache(
                    LibraryCacheEntry::fromByteArray(libData, itemToDelete.uid.toByteArray()));
            if (lib) {
                lib->deleteLibrary();
                delete lib;
            }
        }
    }
}

bool DeleteItemsQuery::deleteChildren(const DeleteItemsQuery::ItemToDelete& itemToDelete,
                                      QLMDB::Transaction& t)
{
    if (!itemToDelete.isLibrary && itemToDelete.childrenOnly) {
        auto uids = children()->getAll(itemToDelete.uid.toByteArray());
        for (const auto& childUid : uids) {
            if (itemToDelete.childFilter) {
                auto itemData = items()->get(t, childUid);
                if (!itemData.isEmpty()) {
                    QScopedPointer<Item> item(
                            Item::decache(ItemCacheEntry::fromByteArray(itemData, childUid)));
                    if (item) {
                        if (itemToDelete.childFilter(item.data())) {
                            m_itemsToDelete << ItemToDelete::itemWithoutNotification(item->uid());
                        }
                    }
                }
            } else {
                // No filter is set - delete all children
                m_itemsToDelete << ItemToDelete::itemWithoutNotification(QUuid(childUid));
            }
        }
        return true;
    }
    return false;
}

void DeleteItemsQuery::run()
{
    QLMDB::Transaction t(*context());
    int i = 0;
    while (i < m_itemsToDelete.length()) {
        const auto& itemToDelete = m_itemsToDelete.at(i);
        i++;
        if (!itemToDelete.uid.isNull()) {
            QQueue<QByteArray> queue;
            queue << itemToDelete.uid.toByteArray(); // NOLINT

            // If the item is a library and we are requested to delete the
            // library directory, do it now:
            deleteLibraryFromDisk(itemToDelete, t);

            // Check if this is a request to delete the children of an item and mark
            // them for deletion if so:
            if (deleteChildren(itemToDelete, t)) {
                queue.clear(); // Empty queue - we've added the selected children to the
                               // instance's list of items to delete
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
        if (itemToDelete.notifyDeletion) {
            emit itemDeleted();
        }
    }
}

DeleteItemsQuery::ItemToDelete DeleteItemsQuery::ItemToDelete::item(const QUuid& uid)
{
    ItemToDelete result;
    result.uid = uid;
    return result;
}

DeleteItemsQuery::ItemToDelete
DeleteItemsQuery::ItemToDelete::itemWithoutNotification(const QUuid& uid)
{
    ItemToDelete result;
    result.uid = uid;
    result.notifyDeletion = false;
    return result;
}

DeleteItemsQuery::ItemToDelete DeleteItemsQuery::ItemToDelete::library(const QUuid& uid,
                                                                       bool deleteLibraryDir)
{
    ItemToDelete result;
    result.uid = uid;
    result.isLibrary = true;
    result.deleteLibraryDir = deleteLibraryDir;
    return result;
}

DeleteItemsQuery::ItemToDelete
DeleteItemsQuery::ItemToDelete::children(const QUuid& uid, DeleteItemsQuery::ItemFilter filter)
{
    ItemToDelete result;
    result.uid = uid;
    result.childrenOnly = true;
    result.childFilter = filter;
    return result;
}

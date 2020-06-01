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

#include <QJsonDocument>
#include <QRandomGenerator>

#include <qlmdb/context.h>
#include <qlmdb/cursor.h>
#include <qlmdb/database.h>
#include <qlmdb/transaction.h>

#include "datamodel/library.h"
#include "datamodel/task.h"
#include "datamodel/todo.h"
#include "datamodel/toplevelitem.h"
#include "datastorage/cache.h"
#include "datastorage/insertorupdateitemsquery.h"

InsertOrUpdateItemsQuery::InsertOrUpdateItemsQuery(QObject* parent)
    : ItemsQuery(parent), m_itemEntries(), m_libEntries()
{
}

/**
 * @brief Add the @p item to the list of items to be cached.
 */
void InsertOrUpdateItemsQuery::add(Item* item, InsertFlags flags)
{
    if (item != nullptr) {
        auto entry = item->encache();
        if (entry.valid) {
            m_itemEntries.append(entry);
            if (flags.testFlag(CalcWeight)) {
                m_calcWeight.insert(item->uid());
            }
            if (flags.testFlag(Save)) {
                m_save.insert(item->uid());
            }
        }
    }
}

/**
 * @brief Add the @p library to the list of libraries to be cached.
 */
void InsertOrUpdateItemsQuery::add(Library* library, InsertFlags flags)
{
    if (library != nullptr) {
        auto entry = library->encache();
        if (entry.valid) {
            m_libEntries.append(entry);
            if (flags.testFlag(Save)) {
                m_save.insert(library->uid());
            }
        }
    }
}

void InsertOrUpdateItemsQuery::run()
{
    QLMDB::Transaction t(*context());
    QLMDB::Cursor itemCursor(t, *items());
    QLMDB::Cursor childrenCursor(t, *children());

    for (auto lib : m_libEntries) {
        auto data = lib.toByteArray();
        auto id = lib.id.toByteArray();

        auto it = itemCursor.findKey(id);
        if (!it.isValid() || it.value() != data) {
            itemCursor.put(id, data);
            markAsChanged(&t, id);
        }
        childrenCursor.put(Cache::RootId, id, QLMDB::Cursor::NoDuplicateData);

        if (m_save.contains(lib.id)) {
            auto library = Library::decache(lib);
            if (library->isValid()) {
                library->save();
                delete library;
            }
        }
    }
    for (auto item : m_itemEntries) {
        if (m_calcWeight.contains(item.id)) {
            auto i = Item::decache(item);
            auto parentId = item.parentId;
            double weight = 0.0;
            auto childIds = children()->getAll(t, parentId.toByteArray());
            for (auto childId : childIds) {
                auto data = items()->get(t, childId);
                if (!data.isNull()) {
                    auto siblingItem = Item::decache(ItemCacheEntry::fromByteArray(data, childId));
                    if (siblingItem) {
                        weight = qMax(weight, siblingItem->weight());
                        delete siblingItem;
                    }
                }
            }
            weight += 1.0 + QRandomGenerator::securelySeeded().generateDouble();
            i->setWeight(weight);
            item = i->encache();
            delete i;
        }
        if (m_save.contains(item.id)) {
            auto i = Item::decache(item);
            if (i->isValid()) {
                i->save();
            }
            delete i;
        }
        auto data = item.toByteArray();
        auto id = item.id.toByteArray();
        auto it = itemCursor.findKey(id);
        if (!it.isValid() || it.value() != data) {
            itemCursor.put(id, data);
            markAsChanged(&t, id);
        }
        childrenCursor.put(item.parentId.toByteArray(), id, QLMDB::Cursor::NoDuplicateData);
    }
}

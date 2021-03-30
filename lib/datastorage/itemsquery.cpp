/*
 * Copyright 2020-2021 Martin Hoeher <martin@rpdev.net>
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

#include <qlmdb/cursor.h>
#include <qlmdb/database.h>

#include <QRandomGenerator>

#include "datamodel/item.h"
#include "datamodel/library.h"
#include "datastorage/itemsquery.h"

/**
 * @brief Constructor.
 */
ItemsQuery::ItemsQuery(QObject* parent)
    : QObject(parent),
      m_context(),
      m_global(),
      m_items(),
      m_children(),
      m_dataChanged(false),
      m_changedLibrariesUids(),
      m_changedParentUids()
{
}

/**
 * @brief Destructor.
 */
ItemsQuery::~ItemsQuery() {}

/**
 * @brief The database containing global settings.
 */
QSharedPointer<QLMDB::Database> ItemsQuery::global() const
{
    return m_global;
}

/**
 * @brief The database containing item data.
 */
QSharedPointer<QLMDB::Database> ItemsQuery::items() const
{
    return m_items;
}

/**
 * @brief The database containing parent/child relationships.
 */
QSharedPointer<QLMDB::Database> ItemsQuery::children() const
{
    return m_children;
}

/**
 * @brief Indicate that the query changed the cache contents.
 *
 * This method shall be called by concrete sub-classes to indicate that the
 * cache contents have been changed.
 * @param changed
 */
void ItemsQuery::setDataChanged(bool changed)
{
    m_dataChanged = changed;
}

/**
 * @brief Returns true if the query has changed the cache.
 *
 * @sa setDataChanged()
 */
bool ItemsQuery::hasDataChanged() const
{
    return m_dataChanged;
}

/**
 * @brief Mark an item as changed.
 *
 * This marks the item with the given @p id as changed. This will cause the
 * librariesChanged() signal to be emitted when the transaction is done.
 */
void ItemsQuery::markAsChanged(QLMDB::Transaction* transaction, QByteArray id)
{
    q_check_ptr(transaction);
    while (!m_changedParentUids.contains(id)) {
        auto entry = m_items->get(*transaction, id);
        if (!entry.isEmpty()) {
            auto itemEntry = ItemCacheEntry::fromByteArray(entry, id);
            if (itemEntry.valid) {
                m_changedParentUids.insert(id);
                id = itemEntry.parentId.toByteArray();
            } else {
                auto libEntry = LibraryCacheEntry::fromByteArray(entry, id);
                if (libEntry.valid) {
                    m_changedLibrariesUids.insert(id);
                    m_changedParentUids.insert(id);
                    setDataChanged(true);
                    break;
                }
            }
        } else {
            break;
        }
    }
}

/**
 * @brief Find a suitable weight for an item to add to the item with the @p parentId.
 *
 * This basically returns the maximum weight of all items below the item with the @p parentId
 * plus some offset, or zero, in case the parent item has no children.
 */
double ItemsQuery::weightForNextItem(const QByteArray& parentId, QLMDB::Transaction& transaction)
{
    // Find the max weight in the target todo list and set it as the todo's one
    bool hasSiblings = false;
    double weight = 0.0;
    QLMDB::Cursor childrenCursor(transaction, *children());
    auto result = childrenCursor.findKey(parentId);
    while (result.isValid()) {
        auto childData = children()->get(transaction, result.value());
        if (!childData.isNull()) {
            QSharedPointer<Item> childItem(
                    Item::decache(ItemCacheEntry::fromByteArray(childData, result.value())));
            if (childItem) {
                if (!hasSiblings) {
                    weight = childItem->weight();
                } else {
                    weight = qMax(weight, childItem->weight());
                }
                hasSiblings = true;
            }
        }
        result = childrenCursor.nextForCurrentKey();
    }
    if (hasSiblings) {
        weight += 1.0 + QRandomGenerator::securelySeeded().generateDouble();
    }
    return weight;
}

/**
 * @brief The context of the cache data base.
 */
QSharedPointer<QLMDB::Context> ItemsQuery::context() const
{
    return m_context;
}

/**
 * @brief Indicate that the query is done.
 *
 * This method shall be called by sub-classes of the ItemsQuery class.
 * It will cause the finished() signal to be emitted, which other classes
 * should use to get informed that the query has been processed.
 */
void ItemsQuery::finish()
{
    if (m_dataChanged) {
        emit dataChanged();
    }
    if (!m_changedLibrariesUids.isEmpty()) {
        QVariantList librariesUids;
        for (auto id : m_changedLibrariesUids) {
            librariesUids << QUuid(id);
        }
        emit librariesChanged(librariesUids);
    }
    emit finished();
}

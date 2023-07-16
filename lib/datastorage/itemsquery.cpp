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
#include <QLoggingCategory>
#include <QSet>
#include <QQueue>

#include <algorithm>

#include "datamodel/item.h"
#include "datamodel/library.h"
#include "datamodel/todo.h"
#include "datastorage/itemsquery.h"

static Q_LOGGING_CATEGORY(log, "OpenTodoList.ItemsQuery", QtWarningMsg);

/**
 * @brief Constructor.
 */
ItemsQuery::ItemsQuery(QObject* parent)
    : QObject(parent),
      m_cache(nullptr),
      m_context(),
      m_global(),
      m_items(),
      m_children(),
      m_dataChanged(false),
      m_isNonDBQuery(false),
      m_changedLibrariesUids(),
      m_changedParentUids(),
      m_parentsMap()
{
}

/**
 * @brief Destructor.
 */
ItemsQuery::~ItemsQuery() {}

/**
 * @brief The Cache the query runs on.
 */
Cache* ItemsQuery::cache() const
{
    return m_cache;
}

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

void ItemsQuery::calculateValues(QLMDB::Transaction& transaction, ItemCacheEntry* entry, Item* item)
{
    q_check_ptr(entry);
    QVariantMap properties;

    ItemPtr itemPtr;

    if (!item) {
        itemPtr.reset(Item::decache(*entry));
        item = itemPtr.data();
    }

    if (item) {
        properties["childrenUpdatedAt"] =
                earliestChildUpdatedAt(transaction, entry->id.toByteArray());
        properties["parents"] = QVariant::fromValue(lookupParents(transaction, item));

        auto todoList = qobject_cast<TodoList*>(item);
        if (todoList) {
            properties["earliestChildDueDate"] =
                    earliestChildDueDate(transaction, entry->id.toByteArray());
        }
        auto todo = qobject_cast<Todo*>(item);
        if (todo != nullptr || todoList != nullptr) {
            auto percentageDone = percentageForListOfItems(transaction, entry->id.toByteArray());
            properties["percentageDone"] = percentageDone.percentageDone;
            properties["numSubtasks"] = percentageDone.numSubtasks;
            properties["numDoneSubtasks"] = percentageDone.numDoneSubtasks;
        }
    }

    entry->calculatedData = properties;
}

/**
 * @brief Calculates the percentage done for a list of checkable items.
 *
 * This calculates the percentage done (as well as number of total and open) items within a list of
 * checkable items. It is applicable e.g. for a TodoList or Todo.
 */
ItemsQuery::PercentageForListOfItems
ItemsQuery::percentageForListOfItems(QLMDB::Transaction& transaction, const QByteArray& todoId)
{
    PercentageForListOfItems result { 0, 0, 0 };

    const auto taskIds = children()->getAll(transaction, todoId);
    if (!taskIds.isEmpty()) {
        int done = 0;
        int total = 0;
        for (const auto& taskId : taskIds) {
            auto data = items()->get(transaction, taskId);
            ItemPtr task(Item::decache(ItemCacheEntry::fromByteArray(data, taskId)));
            if (task) {
                ++total;
                if (task->property("done").toBool()) {
                    ++done;
                }
            }
        }
        result.numSubtasks = total;
        result.numDoneSubtasks = done;
        if (total > 0) {
            result.percentageDone = done * 100 / total;
        }
    }
    return result;
}

/**
 * @brief Calculate the earliest due date among the direct children of an item.
 *
 * This function tries to look up the earliest due date among all children of the item identified by
 * the  @p parentId. If the item has no children or none of them has a due date, an invalid date
 * time object is returned.
 */
QDateTime ItemsQuery::earliestChildDueDate(QLMDB::Transaction& transaction,
                                           const QByteArray& parentId)
{
    QDateTime result;
    const auto childIds = children()->getAll(transaction, parentId);

    for (const auto& childId : childIds) {
        auto data = items()->get(transaction, childId);
        ItemPtr item(Item::decache(ItemCacheEntry::fromByteArray(data, childId)));
        if (item && item->parentId() == QUuid(parentId)) {
            auto complexItem = qSharedPointerCast<ComplexItem>(item);
            if (complexItem) {
                auto val = complexItem->property("done");
                if (val.isValid() && val.typeId() == QMetaType::Bool && val.toBool()) {
                    // Ignore items which have a "done" state which is set to true
                    continue;
                }
                auto dueTo = complexItem->effectiveDueTo();
                if (dueTo.isValid()) {
                    if (!result.isValid() || dueTo < result) {
                        result = dueTo;
                    }
                }
            }
        }
    }

    return result;
}

/**
 * @brief Calculcate the most recent "updatedAt" time of any of the children of the item.
 *
 * This method searches (recursively) for any children of the item with the given
 * @p parentId and returns the most recent updatedAt property from all of them.
 *
 * If the item does not have children, then an invalid result is returned.
 */
QDateTime ItemsQuery::earliestChildUpdatedAt(QLMDB::Transaction& transaction,
                                             const QByteArray& parentId)
{
    QDateTime result;
    const auto childIds = children()->getAll(transaction, parentId);
    QUuid parentIdAsUuid(parentId);

    for (const auto& childId : childIds) {
        auto data = items()->get(transaction, childId);
        ItemPtr item(Item::decache(ItemCacheEntry::fromByteArray(data, childId)));
        if (item && item->parentId() == parentIdAsUuid) {
            auto itemUpdatedAt = item->updatedAt();
            if (!result.isValid() || (itemUpdatedAt.isValid() && itemUpdatedAt > result)) {
                result = itemUpdatedAt;
            }
            // Check children:
            auto childrenUpdatedAt = earliestChildUpdatedAt(transaction, childId);
            if (!result.isValid() || (childrenUpdatedAt.isValid() && childrenUpdatedAt > result)) {
                result = childrenUpdatedAt;
            }
        }
    }

    return result;
}

/**
 * @brief Call a function for all children of an item.
 *
 * This calls the callable @p fn for all children of the item with the uid @p itemId. The @p
 * transaction is used to run any actions against the cache. The @p mode determines which children
 * are evaluated, e.g. if we run on direct children only or if the function shall be called
 * recursively for all children.
 *
 * This method can be used for both querying information as well as changing child items. If a
 * particular item shall be saved back, the callable shall return true.
 */
void ItemsQuery::forAllChildren(QLMDB::Transaction& transaction, const QByteArray& itemId,
                                ChildRecursionMode mode,
                                std::function<bool(QSharedPointer<Item>)> fn)
{
    QSet<QByteArray> visitedChildren; // To protect against circles
    QQueue<QByteArray> parents;

    parents.enqueue(itemId);

    while (!parents.isEmpty()) {
        auto id = parents.dequeue();
        visitedChildren.insert(id);
        const auto childIds = children()->getAll(transaction, id);
        for (const auto& childId : childIds) {
            // Check if we already visited this one:
            if (visitedChildren.contains(childId)) {
                continue;
            }
            // Load the item and apply the function on it:
            auto item = itemFromCache(transaction, QUuid(childId));
            if (item) {
                if (fn(item)) {
                    // If "fn" returns true, this is an indication that the item was changes.
                    // Hence, write it back to cache:
                    itemToCache(transaction, item);
                }
                // Check for the children of the item later:
                parents.enqueue(item->uid().toByteArray());
            }
        }
    }
}

/**
 * @brief Load an item from the cache, using the specified transaction.
 */
QSharedPointer<Item> ItemsQuery::itemFromCache(QLMDB::Transaction& t, const QUuid& itemUid)
{
    auto data = items()->get(t, itemUid.toByteArray());
    if (!data.isNull()) {
        return ItemPtr(Item::decache(ItemCacheEntry::fromByteArray(data, itemUid.toByteArray())));
    }
    return nullptr;
}

/**
 * @brief Write an item to the cache.
 *
 * This writes the given @p item to the cache, using the transaction @p t.
 */
void ItemsQuery::itemToCache(QLMDB::Transaction& t, QSharedPointer<Item> item)
{
    if (!item) {
        return;
    }
    auto cacheEntry = item->encache();
    auto data = cacheEntry.toByteArray();
    auto id = cacheEntry.id.toByteArray();
    QLMDB::Cursor itemCursor(t, *items());
    QLMDB::Cursor childrenCursor(t, *children());
    auto it = itemCursor.findKey(id);
    if (!it.isValid() || it.value() != data) {
        itemCursor.put(id, data);
        markAsChanged(&t, id);
        item->save(); // Also save to disk!
    }
    childrenCursor.put(cacheEntry.parentId.toByteArray(), id, QLMDB::Cursor::NoDuplicateData);
}

/**
 * @brief Get a list of UIDs of the parents of the item.
 *
 * This returns a list of UIDs of the parents of the @p item. The first entry in the list
 * is the UID of the library to which the item belongs, followed by the UIDs of the other parents
 * (if any) up to the direct one of the item.
 *
 * If an error occurred, an empty list is returned.
 */
QVector<QUuid> ItemsQuery::lookupParents(QLMDB::Transaction& t, const Item* item)
{
    QVector<QUuid> result;
    if (m_parentsMap.contains(item->uid())) {
        // We already looked up parents for this item. Restore from the local cache
        for (auto uid = item->parentId(); !uid.isNull(); uid = m_parentsMap.value(uid, QUuid())) {
            result.append(uid);
        }
        std::reverse(result.begin(), result.end());
    } else {
        m_parentsMap[item->uid()] = item->parentId();
        auto topLevelItem = qobject_cast<const TopLevelItem*>(item);
        if (topLevelItem) {
            // The item is a top level item. It does not have any other parents than the library it
            // belongs to. Hence, create a new list containing only the library UID.
            result.append(item->parentId());
        } else {
            // The item is an item which has another item as direct parent. Hence, look up the other
            // item, retrieve its parents and then append the parent item's UID.
            auto parentItem = itemFromCache(t, item->parentId());
            if (parentItem) {
                result = lookupParents(t, parentItem.data());
                if (result.isEmpty()) {
                    // This should not happen! Propagate through the empty list (which indicates an
                    // error).
                    return result;
                }
                result.append(item->parentId());
            } else {
                qCWarning(log) << "Failed to lookup parent item" << item->parentId() << "for item"
                               << item->uid();
            }
        }
    }
    return result;
}

/**
 * @brief Indicates if this query is unrelated to the actual database.
 *
 * If this is set to true, the query will be run via a dedicated thread pool. This is useful to
 * parallelize and not block any work that could happen in parallel on the database.
 *
 * The default is false (i.e. queries are run in the thread pool dedicated for work on the DB).
 */
bool ItemsQuery::isNonDBQuery() const
{
    return m_isNonDBQuery;
}

/**
 * @brief Set if the query is not related to the actual database.
 */
void ItemsQuery::setIsNonDBQuery(bool newIsNonDBQuery)
{
    m_isNonDBQuery = newIsNonDBQuery;
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
        for (auto& id : qAsConst(m_changedLibrariesUids)) {
            librariesUids << QUuid(id);
        }
        emit librariesChanged(librariesUids);
    }
    emit finished();
}

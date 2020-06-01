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

#include <qlmdb/cursor.h>
#include <qlmdb/database.h>
#include <qlmdb/transaction.h>

#include "datamodel/item.h"
#include "datamodel/todo.h"
#include "datastorage/getitemsquery.h"

GetItemsQuery::GetItemsQuery(QObject* parent)
    : ItemsQuery(parent),
      m_parents(),
      m_recursive(false),
      m_transaction(nullptr),
      m_calculateProperties(false)
{
    qRegisterMetaType<ItemCacheEntry>();
}

void GetItemsQuery::run()
{
    QVariantList result;
    QLMDB::Transaction t(*context());
    m_transaction = &t;
    QLMDB::Cursor itemsCursor(t, *items());
    QLMDB::Cursor childrenCursor(t, *children());

    if (m_parents.isEmpty()) {
        // Retrieve all items
        auto it = itemsCursor.first();
        while (it.isValid()) {
            auto entry = ItemCacheEntry::fromByteArray(it.value(), it.key());
            if (entry.valid) {
                if (m_itemFilter) {
                    auto item = ItemPtr(Item::decache(entry));
                    if (m_itemFilter(item, this)) {
                        calculateValues(&entry, item.data());
                        result << QVariant::fromValue(entry);
                    }
                } else {
                    calculateValues(&entry);
                    result << QVariant::fromValue(entry);
                }
            }
            it = itemsCursor.next();
        }
    } else {
        QQueue<QByteArray> queue;
        for (auto id : m_parents) {
            queue.enqueue(id.toByteArray());
        }
        while (!queue.isEmpty()) {
            auto parentId = queue.dequeue();
            auto pit = childrenCursor.findKey(parentId);
            while (pit.isValid()) {
                auto childId = pit.value();
                if (m_recursive) {
                    queue.enqueue(childId);
                }
                auto it = itemsCursor.findKey(childId);
                auto entry = ItemCacheEntry::fromByteArray(it.value(), it.key());
                if (entry.valid) {
                    if (m_itemFilter) {
                        auto item = ItemPtr(Item::decache(entry));
                        if (m_itemFilter(item, this)) {
                            calculateValues(&entry, item.data());
                            result << QVariant::fromValue(entry);
                        }
                    } else {
                        calculateValues(&entry);
                        result << QVariant::fromValue(entry);
                    }
                }
                pit = childrenCursor.nextForCurrentKey();
            }
        }
    }
    m_transaction = nullptr;
    emit itemsAvailable(result);
}

void GetItemsQuery::calculateValues(ItemCacheEntry* entry, Item* item)
{
    q_check_ptr(entry);
    if (m_calculateProperties) {
        QVariantMap properties;

        if (!item) {
            ItemPtr itemPtr(Item::decache(*entry));
            item = itemPtr.data();
        }

        auto todo = qobject_cast<Todo*>(item);
        if (todo != nullptr) {
            properties["percentageDone"] = percentageForTodo(entry->id.toByteArray());
        }

        entry->calculatedData = properties;
    }
}

int GetItemsQuery::percentageForTodo(const QByteArray& todoId)
{
    auto taskIds = children()->getAll(*m_transaction, todoId);
    if (taskIds.isEmpty()) {
        return 0;
    } else {
        int done = 0;
        for (auto taskId : taskIds) {
            auto data = items()->get(*m_transaction, taskId);
            ItemPtr task(Item::decache(ItemCacheEntry::fromByteArray(data, taskId)));
            if (task && task->property("done").toBool()) {
                ++done;
            }
        }
        return done * 100 / taskIds.length();
    }
}

std::function<bool(ItemPtr, GetItemsQuery*)> GetItemsQuery::itemFilter() const
{
    return m_itemFilter;
}

void GetItemsQuery::setItemFilter(const std::function<bool(ItemPtr, GetItemsQuery*)>& itemFilter)
{
    m_itemFilter = itemFilter;
}

/**
 * @brief Retrieve items recursively.
 *
 * If this property is set to true, the retrieval is done recursively.
 * Otherwise, only the direct children of the given parents are retrieved.
 */
bool GetItemsQuery::recursive() const
{
    return m_recursive;
}

/**
 * @brief Set if the retrieval shall be recursive.
 */
void GetItemsQuery::setRecursive(bool recursive)
{
    m_recursive = recursive;
}

GetItemsQuery::ChildrenGenerator GetItemsQuery::childrenOf(const QUuid& id)
{
    return ChildrenGenerator(this, id);
}

/**
 * @brief The list of parents to query items for.
 *
 * The parents is a list of UUIDs of parent items or libraries for which to
 * query items for. In case of an empty list, all items are returned.
 */
QList<QUuid> GetItemsQuery::parents() const
{
    return m_parents;
}

/**
 * @brief Set the list of parent UUIDs to query items for.
 */
void GetItemsQuery::setParents(const QList<QUuid>& parents)
{
    m_parents = parents;
}

/**
 * @brief The parent to query items for.
 *
 * This is the UUID of the parent for which to query items. Basically, this is
 * the first entry in the list of @sa parents(). If that list is empty, this
 * property equals to an invalid or null UUID.
 */
QUuid GetItemsQuery::parent() const
{
    if (m_parents.length() > 0) {
        return m_parents.at(0);
    } else {
        return QUuid();
    }
}

/**
 * @brief Set the parent to query items for.
 */
void GetItemsQuery::setParent(const QUuid& parent)
{
    m_parents.clear();
    m_parents << parent;
}

const ItemPtr& GetItemsQuery::ChildrenIterator::operator*() const
{
    return m_item;
}

GetItemsQuery::ChildrenIterator::ChildrenIterator()
    : m_childrenCursor(nullptr), m_dataCursor(nullptr), m_id(), m_item()
{
}

GetItemsQuery::ChildrenIterator::ChildrenIterator(QLMDB::Cursor* childrenCursor,
                                                  QLMDB::Cursor* dataCursor, const QUuid& id)
    : m_childrenCursor(childrenCursor), m_dataCursor(dataCursor), m_id(id), m_item()
{
    ++(*this);
}

GetItemsQuery::ChildrenIterator::~ChildrenIterator() {}

GetItemsQuery::ChildrenIterator& GetItemsQuery::ChildrenIterator::operator++()
{
    if (m_childrenCursor != nullptr) {
        m_item.reset();
        QLMDB::Cursor::FindResult it;
        if (m_childrenCursor->current().isValid()) {
            it = m_childrenCursor->nextForCurrentKey();
        } else {
            it = m_childrenCursor->findKey(m_id.toByteArray());
        }
        while (!m_item && it.isValid()) {
            auto dit = m_dataCursor->findKey(it.value());
            if (dit.isValid()) {
                auto entry = ItemCacheEntry::fromByteArray(dit.value(), it.key());
                m_item = ItemPtr(Item::decache(entry));
            }
            it = m_childrenCursor->nextForCurrentKey();
        }
        if (!m_item) {
            m_childrenCursor = nullptr;
        }
    }
    return *this;
}

bool GetItemsQuery::ChildrenIterator::operator!=(const GetItemsQuery::ChildrenIterator& other)
{
    return m_item || other.m_item;
}

bool GetItemsQuery::ChildrenIterator::operator==(const GetItemsQuery::ChildrenIterator& other)
{
    return !(*this != other);
}

GetItemsQuery::ChildrenIterator GetItemsQuery::ChildrenGenerator::begin()
{
    auto childrenCursor = new QLMDB::Cursor(*m_query->m_transaction, *m_query->children());
    auto dataCursor = new QLMDB::Cursor(*m_query->m_transaction, *m_query->items());
    return ChildrenIterator(childrenCursor, dataCursor, m_id);
}

GetItemsQuery::ChildrenIterator GetItemsQuery::ChildrenGenerator::end()
{
    return ChildrenIterator();
}

GetItemsQuery::ChildrenGenerator::ChildrenGenerator(GetItemsQuery* query, const QUuid& id)
    : m_query(query), m_id(id)
{
}

/**
 * @brief Calculate additional properties for returned items.
 *
 * If this property is set to true, the query will calculate additional
 * properties for items returned.
 */
bool GetItemsQuery::calculateProperties() const
{
    return m_calculateProperties;
}

void GetItemsQuery::setCalculateProperties(bool calculateProperties)
{
    m_calculateProperties = calculateProperties;
}

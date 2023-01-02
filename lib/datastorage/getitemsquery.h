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

#ifndef DATASTORAGE_GETITEMSQUERY_H_
#define DATASTORAGE_GETITEMSQUERY_H_

#include <QObject>

#include <functional>
#include <iterator>

#include "datamodel/item.h"
#include "datastorage/itemsquery.h"

namespace QLMDB {
class Cursor;
class Transaction;
}

class GetItemsQuery : public ItemsQuery
{
    Q_OBJECT
public:
    class ChildrenGenerator;

    class ChildrenIterator
    {
        friend class ChildrenGenerator;

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = ItemPtr;
        using difference_type = int;
        using pointer = ItemPtr*;
        using reference = ItemPtr&;

        ChildrenIterator();
        ChildrenIterator(const ChildrenIterator& other) = default;
        virtual ~ChildrenIterator();
        const ItemPtr& operator*() const;
        ChildrenIterator& operator++();
        bool operator!=(const ChildrenIterator& other);
        bool operator==(const ChildrenIterator& other);

    private:
        QSharedPointer<QLMDB::Cursor> m_childrenCursor;
        QSharedPointer<QLMDB::Cursor> m_dataCursor;
        QUuid m_id;
        ItemPtr m_item;

        explicit ChildrenIterator(QLMDB::Cursor* childrenCursor, QLMDB::Cursor* dataCursor,
                                  const QUuid& id);
    };

    class ChildrenGenerator
    {
        friend class GetItemsQuery;

    public:
        ChildrenGenerator(const ChildrenGenerator& other) = default;
        ChildrenIterator begin();
        ChildrenIterator end();

    private:
        GetItemsQuery* m_query;
        QUuid m_id;

        explicit ChildrenGenerator(GetItemsQuery* query, const QUuid& id);
    };

    explicit GetItemsQuery(QObject* parent = nullptr);

    QList<QUuid> parents() const;
    void setParents(const QList<QUuid>& parents);

    QUuid parent() const;
    void setParent(const QUuid& parent);

    bool recursive() const;
    void setRecursive(bool recursive);

    ChildrenGenerator childrenOf(const QUuid& id);

    std::function<bool(ItemPtr, GetItemsQuery*)> itemFilter() const;
    void setItemFilter(const std::function<bool(ItemPtr, GetItemsQuery*)>& itemFilter);

signals:

    /**
     * @brief The items have been loaded.
     *
     * This signal is emitted to indicate that the requested items have
     * been loaded from the cache. The @p items list contains the cache
     * entries, which can be turned to item objects by calling Item::decache()
     * on each entry.
     */
    void itemsAvailable(QVariantList items);

    // ItemsQuery interface
protected:
    void run() override;

private:
    QList<QUuid> m_parents;
    bool m_recursive;
    QLMDB::Transaction* m_transaction;
    std::function<bool(ItemPtr, GetItemsQuery*)> m_itemFilter;
};

#endif // DATASTORAGE_GETITEMSQUERY_H_

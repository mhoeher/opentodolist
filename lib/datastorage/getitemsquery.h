#ifndef GETITEMSQUERY_H
#define GETITEMSQUERY_H

#include <functional>

#include <QObject>

#include "datamodel/item.h"
#include "datastorage/itemsquery.h"

namespace QLMDB
{
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
        ChildrenIterator();
        ChildrenIterator(const ChildrenIterator &other) = default;
        virtual ~ChildrenIterator();
        const ItemPtr& operator*() const;
        ChildrenIterator& operator ++();
        bool operator !=(const ChildrenIterator &other);

    private:
        QLMDB::Cursor *m_childrenCursor;
        QLMDB::Cursor *m_dataCursor;
        ItemPtr m_item;

        explicit ChildrenIterator(
                QLMDB::Cursor* childrenCursor,
                QLMDB::Cursor *dataCursor,
                const QUuid &id);
    };

    class ChildrenGenerator
    {
        friend class GetItemsQuery;
    public:
        ChildrenGenerator(const ChildrenGenerator &other) = default;
        ChildrenIterator begin();
        ChildrenIterator end();

    private:
        GetItemsQuery *m_query;
        QUuid m_id;

        explicit ChildrenGenerator(GetItemsQuery *query, const QUuid &id);
    };

    explicit GetItemsQuery(QObject *parent = nullptr);

    QList<QUuid> parents() const;
    void setParents(const QList<QUuid> &parents);

    QUuid parent() const;
    void setParent(const QUuid &parent);

    bool recursive() const;
    void setRecursive(bool recursive);

    ChildrenGenerator childrenOf(const QUuid &id);

    std::function<bool (ItemPtr, GetItemsQuery *)> itemFilter() const;
    void setItemFilter(const std::function<bool (ItemPtr, GetItemsQuery *)> &itemFilter);

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
    QLMDB::Transaction *m_transaction;
    std::function<bool(ItemPtr, GetItemsQuery*)> m_itemFilter;
};

#endif // GETITEMSQUERY_H

#include <qlmdb/transaction.h>
#include <qlmdb/cursor.h>

#include "datamodel/item.h"
#include "datastorage/getitemsquery.h"

GetItemsQuery::GetItemsQuery(QObject *parent) :
    ItemsQuery(parent)
{
    qRegisterMetaType<ItemCacheEntry>();
}

void GetItemsQuery::run()
{
    QVariantList result;
    QLMDB::Transaction t(*context());
    QLMDB::Cursor itemsCursor(t, *items());
    QLMDB::Cursor childrenCursor(t, *children());

    if (m_parents.isEmpty()) {
        // Retrieve all items
        auto it = itemsCursor.first();
        while (it.isValid()) {
            auto entry = ItemCacheEntry::fromByteArray(it.value(), it.key());
            if (entry.valid) {
                result << QVariant::fromValue(entry);
            }
            it = itemsCursor.next();
        }
    } else {
        for (auto parentId : m_parents) {
            auto pit = childrenCursor.findKey(parentId.toByteArray());
            while (pit.isValid()) {
                auto it = itemsCursor.findKey(pit.value());
                auto entry = ItemCacheEntry::fromByteArray(
                            it.value(), it.key());
                if (entry.valid) {
                    result << QVariant::fromValue(entry);
                }
                pit = childrenCursor.nextForCurrentKey();
            }
        }
    }
    emit itemsAvailable(result);
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
void GetItemsQuery::setParents(const QList<QUuid> &parents)
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
void GetItemsQuery::setParent(const QUuid &parent)
{
    m_parents.clear();
    m_parents << parent;
}

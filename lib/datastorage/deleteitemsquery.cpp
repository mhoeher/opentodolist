#include <QQueue>

#include <qlmdb/transaction.h>
#include <qlmdb/database.h>

#include "datamodel/item.h"
#include "datamodel/library.h"
#include "datastorage/deleteitemsquery.h"

DeleteItemsQuery::DeleteItemsQuery(QObject *parent) :
    ItemsQuery(parent),
    m_uid(),
    m_isLibrary(false)
{

}

void DeleteItemsQuery::deleteItem(const Item *item)
{
    if (item != nullptr) {
        m_uid = item->uid();
        m_isLibrary = false;
    }
}

void DeleteItemsQuery::deleteLibrary(const Library *library)
{
    if (library != nullptr) {
        m_uid = library->uid();
        m_isLibrary = true;
    }
}

void DeleteItemsQuery::run()
{
    if (!m_uid.isNull()) {
        QQueue<QByteArray> queue;
        queue << m_uid.toByteArray();

        QLMDB::Transaction t(*context());

        while (!queue.isEmpty()) {
            auto nextId = queue.dequeue();
            if (!m_isLibrary) {
                // When we delete single items, we have to do it recursively
                // and on disk.
                // Otherwise, orphaned item files will remain in the library
                // directory and added back on every load, however, they will
                // be unreachable and hence spam our cache.
                auto itemData = items()->get(t, nextId);
                if (!itemData.isEmpty()) {
                    auto item = Item::decache(
                                ItemCacheEntry::fromByteArray(
                                    itemData, nextId));
                    if (item != nullptr) {
                        item->deleteItem();
                        delete item;
                    }
                    setDataChanged();
                }
            } else {
                if (!items()->get(t, nextId).isNull()) {
                    setDataChanged();
                }
            }
            items()->remove(t, nextId);
            auto keys = children()->getAll(nextId);
            queue.append(keys);
            children()->remove(t, nextId);
        }

        emit itemDeleted();
    }
}

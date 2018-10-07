#include <QJsonDocument>

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

InsertOrUpdateItemsQuery::InsertOrUpdateItemsQuery(QObject *parent) :
    ItemsQuery(parent),
    m_itemEntries(),
    m_libEntries()
{

}


/**
 * @brief Add the @p item to the list of items to be cached.
 */
void InsertOrUpdateItemsQuery::add(Item *item)
{
    if (item != nullptr) {
        auto entry = item->encache();
        if (entry.valid) {
            m_itemEntries.append(entry);
        }
    }
}


/**
 * @brief Add the @p library to the list of libraries to be cached.
 */
void InsertOrUpdateItemsQuery::add(Library *library)
{
    if (library != nullptr) {
        auto entry = library->encache();
        if (entry.valid) {
            m_libEntries.append(entry);
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
            setDataChanged();
            itemCursor.put(id, data);
        }
        childrenCursor.put(Cache::RootId, id,
                           QLMDB::Cursor::NoDuplicateData);
    }
    for (auto item : m_itemEntries) {
        auto data = item.toByteArray();
        auto id = item.id.toByteArray();
        auto it = itemCursor.findKey(id);
        if (!it.isValid() || it.value() != data) {
            setDataChanged();
            itemCursor.put(id, data);
        }
        childrenCursor.put(item.parentId.toByteArray(), id,
                           QLMDB::Cursor::NoDuplicateData);
    }
}

#include <QJsonDocument>
#include <QLoggingCategory>

#include <qlmdb/cursor.h>
#include <qlmdb/database.h>
#include <qlmdb/transaction.h>

#include "datastorage/cache.h"
#include "datastorage/librariesitemsquery.h"

static Q_LOGGING_CATEGORY(log, "OpenTodoList.LibrariesItemsQuery", QtDebugMsg)


LibrariesItemsQuery::LibrariesItemsQuery(QObject *parent) :
    ItemsQuery(parent)
{
    qRegisterMetaType<LibraryCacheEntry>();
}


void LibrariesItemsQuery::run()
{
    QVariantList result;
    QLMDB::Transaction t(*context());
    QLMDB::Cursor itemsCursor(t, *items());
    QLMDB::Cursor childrenCursor(t, *children());

    auto it = childrenCursor.findKey(Cache::RootId);
    while (it.isValid()) {
        auto item = itemsCursor.findKey(it.value());
        if (item.isValid()) {
            auto entry = LibraryCacheEntry::fromByteArray(
                        item.value(), item.key());
            if (entry.valid) {
                result << QVariant::fromValue(entry);
            }
        } else {
            qCWarning(log) << "Failed to find item"
                           << item.value()
                           << "in item cache!";
            items()->remove(t, Cache::RootId, item.key());
        }
        it = childrenCursor.nextForCurrentKey();
    }
    emit librariesAvailable(result);
}

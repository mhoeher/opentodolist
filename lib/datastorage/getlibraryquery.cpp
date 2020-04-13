#include <qlmdb/database.h>

#include "datamodel/library.h"
#include "datastorage/getlibraryquery.h"

GetLibraryQuery::GetLibraryQuery(QObject *parent) : ItemsQuery(parent), m_uid() {}

QUuid GetLibraryQuery::uid() const
{
    return m_uid;
}

void GetLibraryQuery::setUid(const QUuid &uid)
{
    m_uid = uid;
}

void GetLibraryQuery::run()
{
    auto uid = m_uid.toByteArray();
    auto data = items()->get(uid);
    if (!data.isNull()) {
        auto entry = LibraryCacheEntry::fromByteArray(data, uid);
        if (entry.valid) {
            emit libraryLoaded(QVariant::fromValue(entry));
        }
    }
}

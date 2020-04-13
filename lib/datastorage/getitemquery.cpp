#include <qlmdb/database.h>

#include "datamodel/item.h"
#include "datastorage/getitemquery.h"

GetItemQuery::GetItemQuery(QObject *parent) : ItemsQuery(parent), m_uid() {}

QUuid GetItemQuery::uid() const
{
    return m_uid;
}

void GetItemQuery::setUid(const QUuid &uid)
{
    m_uid = uid;
}

void GetItemQuery::run()
{
    auto uid = m_uid.toByteArray();
    auto data = items()->get(uid);
    if (!data.isNull()) {
        auto entry = ItemCacheEntry::fromByteArray(data, uid);
        if (entry.valid) {
            emit itemLoaded(QVariant::fromValue(entry));
        }
    }
}

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

#include <QJsonDocument>
#include <QLoggingCategory>

#include <qlmdb/cursor.h>
#include <qlmdb/database.h>
#include <qlmdb/transaction.h>

#include "datamodel/item.h"
#include "datamodel/toplevelitem.h"
#include "datastorage/cache.h"
#include "datastorage/librariesitemsquery.h"

static Q_LOGGING_CATEGORY(log, "OpenTodoList.LibrariesItemsQuery", QtDebugMsg)

        LibrariesItemsQuery::LibrariesItemsQuery(QObject *parent)
    : ItemsQuery(parent), m_includeCalculatedValues(false)
{
    qRegisterMetaType<LibraryCacheEntry>();
}

void LibrariesItemsQuery::run()
{
    QVariantList result;
    QLMDB::Transaction t(*context());
    QLMDB::Cursor itemsCursor(t, *items());
    QLMDB::Cursor childrenCursor(t, *children());
    QLMDB::Cursor topLevelItemIdsCursor(t, *children());

    auto it = childrenCursor.findKey(Cache::RootId);
    while (it.isValid()) {
        auto item = itemsCursor.findKey(it.value());
        if (item.isValid()) {
            auto entry = LibraryCacheEntry::fromByteArray(item.value(), item.key());
            if (entry.valid) {
                if (m_includeCalculatedValues) {
                    QSet<QString> tags;
                    auto topIt = topLevelItemIdsCursor.findKey(item.key());
                    while (topIt.isValid()) {
                        auto data = items()->get(topIt.value());
                        if (!data.isNull()) {
                            auto itemEntry = ItemCacheEntry::fromByteArray(data, item.key());
                            if (itemEntry.valid) {
                                ItemPtr item_(Item::decache(itemEntry));
                                auto topLevelItem = item_.objectCast<TopLevelItem>();
                                if (topLevelItem != nullptr) {
                                    for (auto tag : topLevelItem->tags()) {
                                        tags.insert(tag);
                                    }
                                }
                            }
                        }
                        topIt = topLevelItemIdsCursor.nextForCurrentKey();
                    }
                    QVariantMap calculated;
                    calculated["tags"] = QStringList(tags.values());
                    entry.calculatedData = calculated;
                }
                result << QVariant::fromValue(entry);
            }
        } else {
            qCWarning(log) << "Failed to find item" << item.value() << "in item cache!";
            items()->remove(t, Cache::RootId, item.key());
        }
        it = childrenCursor.nextForCurrentKey();
    }
    emit librariesAvailable(result);
}

bool LibrariesItemsQuery::includeCalculatedValues() const
{
    return m_includeCalculatedValues;
}

void LibrariesItemsQuery::setIncludeCalculatedValues(bool includeCalculatedValues)
{
    m_includeCalculatedValues = includeCalculatedValues;
}

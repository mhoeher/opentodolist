/*
 * Copyright 2021 Martin Hoeher <martin@rpdev.net>
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

#include "copyitemquery.h"

#include <QDir>
#include <QLoggingCategory>
#include <QMap>
#include <QQueue>
#include <QSet>

#include <qlmdb/context.h>
#include <qlmdb/cursor.h>
#include <qlmdb/database.h>
#include <qlmdb/transaction.h>

#include "datamodel/library.h"
#include "datamodel/item.h"

static Q_LOGGING_CATEGORY(log, "OpenTodoList.CopyItemQuery", QtDebugMsg);

/**
 * @brief Copy items.
 *
 * This query is used to copy an item recursively either within a library or from one library to
 * another one.
 */

/**
 * @brief Constructor.
 */
CopyItemQuery::CopyItemQuery(QObject* parent)
    : ItemsQuery(parent),
      m_sourceItemUid(),
      m_targetLibraryUid(),
      m_targetPath(),
      m_targetParentItemUid()
{
}

/**
 * @brief Copy an item and its children.
 *
 * This creates a copy of the @p item in the @p targetLibrary (which can be the library where the
 * item is contained or another, unrelated one). If the item is not a TopLevelItem, the @p
 * parentItem must be the item the copy of the item shall be parented to.
 */
void CopyItemQuery::copyItem(Item* item, Library* targetLibrary, Item* parentItem)
{
    q_check_ptr(item);
    q_check_ptr(targetLibrary);
    m_sourceItemUid = item->uid();
    m_targetLibraryUid = targetLibrary->uid();
    m_targetPath = targetLibrary->newItemLocation();
    if (parentItem) {
        m_targetParentItemUid = parentItem->uid();
    }
}

void CopyItemQuery::run()
{
    QLMDB::Transaction t(*context());
    QLMDB::Cursor childrenCursor(t, *children());
    if (!m_targetPath.mkpath(".")) {
        qCWarning(log) << "Failed to create" << m_targetPath << "for copying item";
        return;
    }

    QQueue<QUuid> itemQueue;
    QSet<QUuid> seenUids;

    // Maps from UIDs of source items to UIDs of parent items in the target location.
    QMap<QUuid, QUuid> parentItemMap;

    // Maps from UIDs of a source item UID to its expected source parent item UID. This is
    // used to detect "stale" entries in the children table.
    QMap<QUuid, QUuid> expectedParentMap;

    itemQueue.enqueue(m_sourceItemUid);
    parentItemMap.insert(m_sourceItemUid, m_targetParentItemUid);

    while (!itemQueue.isEmpty()) {
        auto srcItemUid = itemQueue.dequeue();
        seenUids.insert(srcItemUid);
        auto srcItem = itemFromCache(t, srcItemUid);
        if (!srcItem) {
            qCWarning(log) << "Item" << srcItemUid << "not found when copying";
            continue;
        }
        if (expectedParentMap.contains(srcItem->uid())
            && srcItem->parentId() != expectedParentMap.value(srcItem->uid())) {
            qCWarning(log) << "Item" << srcItem->uid() << "has changed its parent - not copying";
            continue;
        }
        QSharedPointer<Item> dstItem(
                srcItem->copyTo(m_targetPath, m_targetLibraryUid, parentItemMap.value(srcItemUid)));
        if (!dstItem) {
            qCWarning(log) << "Failed to copy" << srcItemUid << m_targetPath;
            continue;
        }
        if (dstItem->parentId() == srcItem->parentId()) {
            // If the copy is in the same container (library, parent todo list, ...) prepend a
            // "Copy of" so user's can distinguish the copy from the original.
            dstItem->setTitle(tr("Copy of") + " " + dstItem->title());
        }
        qCDebug(log) << "Copied item" << srcItemUid << "to" << dstItem->uid();
        if (!dstItem->save()) {
            qCWarning(log) << "Failed to save copy" << dstItem->uid() << "of item" << srcItemUid;
            continue;
        }
        {
            auto value = dstItem->encache().toByteArray();
            auto key = dstItem->uid().toByteArray();
            items()->put(t, key, value);
            childrenCursor.put(dstItem->parentId().toByteArray(), key,
                               QLMDB::Cursor::NoDuplicateData);
        }
        if (srcItemUid == m_sourceItemUid) {
            // This is the "root" item we copied - mark it as changed so e.g. views will update.
            markAsChanged(&t, dstItem->uid().toByteArray());
        }
        // Find all children of the current item and also copy them:
        const auto childItemUids = children()->getAll(t, srcItemUid.toByteArray());
        for (const auto& childItemUid : childItemUids) {
            if (seenUids.contains(childItemUid)) {
                qCWarning(log) << "Item" << childItemUid
                               << "has already been visited for copying - circular item reference?";
                continue;
            }
            expectedParentMap.insert(childItemUid, srcItem->uid());
            parentItemMap.insert(childItemUid, dstItem->uid());
            itemQueue.enqueue(childItemUid);
        }
    }
}

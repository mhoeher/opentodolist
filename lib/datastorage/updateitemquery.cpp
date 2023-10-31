/*
 * Copyright 2023 Martin Hoeher <martin@rpdev.net>
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

#include "updateitemquery.h"

#include <QJSEngine>
#include <QLoggingCategory>

#include <qlmdb/cursor.h>
#include <qlmdb/database.h>
#include <qlmdb/transaction.h>

#include "datamodel/item.h"

static Q_LOGGING_CATEGORY(log, "OpenTodoList.UpdateItemQuery", QtDebugMsg);

/**
 * @brief A script that marks all items within an item as done.
 */
const char* UpdateItemQuery::MarkAllTodosAndTasksAsDone = R"(
function updateItem(item) {
    if (typeof(item?.done) == "boolean") {
        item.done = true;
    }
}
)";

/**
 * @brief A script that marks all items within an item as undone.
 */
const char* UpdateItemQuery::MarkAllTodosAndTasksAsUndone = R"(
function updateItem(item) {
    if (typeof(item?.done) == "boolean") {
        item.done = false;
    }
}
)";

/**
 * @brief Contructor.
 */
UpdateItemQuery::UpdateItemQuery(QObject* parent) : ItemsQuery { parent }, m_uid(), m_script() {}

/**
 * @brief The UID of the item to update.
 */
QUuid UpdateItemQuery::uid() const
{
    return m_uid;
}

/**
 * @brief Set the UID of the item to update.
 */
void UpdateItemQuery::setUid(const QUuid& newUid)
{
    if (m_uid == newUid)
        return;
    m_uid = newUid;
    emit uidChanged();
}

/**
 * @brief A script to update the item and its children.
 *
 * This string should be a JavaScript, which defines a function
 * `updateItem()`, which gets - as its only parameter - the item to
 * be updated. The item is an instance of the Item class (or a concrete subclass). The function
 * can update the item as needed.
 *
 * The function will be called for the item set in the query and all or its children. Hence,
 * it should check on the type of the item it gets.
 */
QString UpdateItemQuery::script() const
{
    return m_script;
}

/**
 * @brief Set the update script that shall be run.
 */
void UpdateItemQuery::setScript(const QString& newScript)
{
    if (m_script == newScript)
        return;
    m_script = newScript;
    emit scriptChanged();
}

/**
 * @brief Implementation of ItemsQuery::run().
 */
void UpdateItemQuery::run()
{
    if (m_uid.isNull() || m_script.isEmpty()) {
        qCDebug(log) << "Query is incomplete - no UID or script set.";
        return;
    }

    QLMDB::Transaction t(*context());
    QLMDB::Cursor itemCursor(t, *items());

    auto item = itemFromCache(t, m_uid);
    if (!item) {
        qCWarning(log) << "No item for uid" << m_uid << "found, aborting query.";
        return;
    }

    QJSEngine engine;
    auto error = engine.evaluate(m_script);
    if (error.isError()) {
        qCWarning(log) << "Error during script evaluation:" << error.toString();
        return;
    }
    auto updateFn = engine.globalObject().property("updateItem");
    if (!updateFn.isCallable()) {
        qCWarning(log) << "Script does not define an updateItem() function.";
        return;
    }

    auto updateItem = [&](ItemPtr item) {
        if (!item) {
            return;
        }
        auto oldData = item->encache();
        // Tell JS that the item is not script owned (otherwise, it would try to delete it):
        QJSEngine::setObjectOwnership(item.data(), QJSEngine::CppOwnership);
        QJSValueList args;
        args.append(engine.newQObject(item.data()));
        auto ret = updateFn.call(args);
        if (!ret.isError()) {
            auto newData = item->encache();
            if (oldData.toByteArray() != newData.toByteArray()) {
                qCDebug(log) << "Item" << item->uid() << "has been updated - saving back to cache.";
                itemToCache(t, item);
            }
        } else {
            qCWarning(log) << "Error during updating item" << item->uid() << ":"
                           << error.toString();
        }
    };

    // Update the top level item:
    updateItem(item);

    // Iterte over all children and update them as well:
    forAllChildren(t, m_uid.toByteArray(), ChildRecursionMode::RecursiveChildren,
                   [&](ItemPtr item) -> bool {
                       updateItem(item);
                       return false;
                   });
}

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

#include "promotetaskquery.h"

#include <qlmdb/transaction.h>
#include <qlmdb/cursor.h>
#include <qlmdb/database.h>

#include "datamodel/task.h"
#include "datamodel/todo.h"
#include "datamodel/todolist.h"

PromoteTaskQuery::PromoteTaskQuery(QObject* parent) : ItemsQuery(parent), m_entry(), m_targetUid()
{
}

void PromoteTaskQuery::promoteTask(Task* task, TodoList* todoList)
{
    Q_CHECK_PTR(task);
    Q_CHECK_PTR(todoList);
    m_entry = task->encache();
    m_targetUid = todoList->uid();
}

void PromoteTaskQuery::run()
{
    QLMDB::Transaction t(*context());
    if (m_entry.valid) {
        QSharedPointer<Item> item(Item::decache(m_entry));
        if (item) {
            auto task = qSharedPointerCast<Task>(item);
            // Unlink from current parent
            QLMDB::Cursor childrenCursor(t, *children());
            auto result =
                    childrenCursor.find(task->todoUid().toByteArray(), task->uid().toByteArray());
            if (result.isValid()) {
                childrenCursor.remove();
            }

            auto targetUid = m_targetUid.toByteArray();

            // "Upgrade" the task to a todo
            QSharedPointer<Todo> todo(new Todo(task->filename()));
            todo->setUid(task->uid());
            todo->setTodoListUid(m_targetUid);
            todo->setDone(task->done());
            todo->setTitle(task->title());
            todo->setWeight(weightForNextItem(targetUid, t));

            // Save the new item:
            auto todoUid = todo->uid().toByteArray();
            items()->put(t, todoUid, todo->encache().toByteArray());
            children()->put(t, targetUid, todoUid);
            // Save to disk:
            todo->save();
            // Mark as changed:
            markAsChanged(&t, todoUid);
        }
    }
}

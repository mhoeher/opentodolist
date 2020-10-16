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

#include "movetodoquery.h"

#include <qlmdb/database.h>
#include <qlmdb/transaction.h>

#include "datamodel/todo.h"
#include "datamodel/todolist.h"

MoveTodoQuery::MoveTodoQuery(QObject* parent) : ItemsQuery(parent), m_entry(), m_targetUid() {}

void MoveTodoQuery::moveTodo(Todo* todo, TodoList* todoList)
{
    Q_CHECK_PTR(todo);
    Q_CHECK_PTR(todoList);
    m_entry = todo->encache();
    m_targetUid = todoList->uid();
}

void MoveTodoQuery::run()
{
    QLMDB::Transaction t(*context());

    if (m_entry.valid) {
        QSharedPointer<Item> item(Item::decache(m_entry));
        if (item) {
            auto todo = qSharedPointerCast<Todo>(item);
            if (todo) {
                // "Unlink" from previous parent:
                children()->remove(t, todo->todoListUid().toByteArray());
                // Set new parent:
                todo->setTodoListUid(m_targetUid);
                // Save to DB:
                items()->put(t, todo->uid().toByteArray(), todo->encache().toByteArray());
                children()->put(t, m_targetUid.toByteArray(), todo->uid().toByteArray());
                // Save to disk:
                todo->save();
                // Mark todo as changed:
                markAsChanged(&t, todo->uid().toByteArray());
            }
        }
    }
}

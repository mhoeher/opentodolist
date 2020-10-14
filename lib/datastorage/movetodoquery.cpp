#include "movetodoquery.h"

#include <qlmdb/database.h>
#include <qlmdb/transaction.h>

#include "datamodel/todo.h"
#include "datamodel/todolist.h"

MoveTodoQuery::MoveTodoQuery(QObject* parent) : ItemsQuery(parent), m_entry(), m_targetUid() {}

void MoveTodoQuery::move(Todo* todo, TodoList* todoList)
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

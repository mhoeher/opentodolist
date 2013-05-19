#include "abstracttodolist.h"

#include "abstracttodo.h"

AbstractTodoList::AbstractTodoList(QObject *parent) :
    QObject(parent)
{
}

const AbstractTodoList::TodoList &AbstractTodoList::todos() const
{
    return m_todos;
}

AbstractTodo *AbstractTodoList::addTodo()
{
    m_todos << new AbstractTodo( this );
    return m_todos.last();
}

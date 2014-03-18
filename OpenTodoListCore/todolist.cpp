/*
 *  OpenTodoList - A todo and task manager
 *  Copyright (C) 2013  Martin Höher <martin@rpdev.net>
 * 
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 * 
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 * 
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "todolist.h"

#include "todo.h"

/**
   @brief Constructor

   Creates a new todo list. The @p key and @p settings arguments are either
   invalid values (in case the todo list is created) or some meaningful
   values in case the list is restored. The @p parent will take ownership
   of this object. The @p type is the type ID of the todo list factory
   creating this object.
 */
TodoList::TodoList(const QString& key, const QString& type,
                                   const QVariant &settings, QObject* parent) :
    QObject(parent),
    m_todos( new TodosList( this ) ),
    m_topLevelTodos( new TodoSortFilterModel( this ) ),
    m_deletedTodos( new TodoSortFilterModel( this ) ),
    m_type( type ),
    m_key( key )
{
    Q_UNUSED( settings );
    m_topLevelTodos->setFilterMode( TodoSortFilterModel::TodoListEntries | TodoSortFilterModel::HideDeleted );
    m_topLevelTodos->setSortMode( TodoSortFilterModel::PrioritySort );
    m_topLevelTodos->setSourceModel( m_todos );

    m_deletedTodos->setFilterMode( TodoSortFilterModel::HideNonDeleted );
    m_deletedTodos->setSourceModel( m_todos );
    
    connect( this, SIGNAL(nameChanged()), this, SIGNAL(changed()) );
}

TodoList::TodosList* TodoList::todos() const
{
    return m_todos;
}

TodoSortFilterModel* TodoList::topLevelTodos() const
{
    return m_topLevelTodos;
}

TodoSortFilterModel *TodoList::deletedTodos() const
{
    return m_deletedTodos;
}

QObject *TodoList::addTodo()
{
    Todo* todo = new Todo( QUuid::createUuid(), this );
    appendTodo( todo );
    return todo;
}

QObject* TodoList::addTodo(const QString& title, QObject* parentTodo)
{
    Todo* result = qobject_cast< Todo* >( addTodo() );
    if ( result ) {
        result->setTitle( title );
        result->setParentTodo( qobject_cast< Todo* >( parentTodo ) );
    }
    return result;
}

/**
   @brief Store additional settings of the todo list

   This method can be overridden to return additional settings of the todo
   list that shall be saved in the application settings.

   By default, it is up to the concrete storage backend to store the settings of
   the todo list. However, some settings might need to be stored inside the
   application settings.

   In a subclass of AbstractTodoList, overriding this function should include
   the (potential) settings of the base class (if any):

   @code
   QVariant MyTodoList::settings() {
     QVariantMap result;
     result.insert( "myValue", 42 );
     QVariant baseSettings = AbstractTodoList::settings();
     result.insert( "base", baseSettings );
     return result;
   }
   @endcode
 */
QVariant TodoList::settings()
{
    return QVariant();
}

const QString& TodoList::name() const
{
    return m_name;
}

void TodoList::setName(const QString& name)
{
    m_name = name;
    emit nameChanged();
}

/**
   @brief The display name of the todo list

   Returns the name of the todo list for displaying. Usually, this will be the
   same as name(). In case the name is empty, a default string is returned.
 */
QString TodoList::displayName() const
{
    if ( m_name.isEmpty() ) {
        return tr( "[Unnamed Todo List]" );
    } else {
        return m_name;
    }
}

void TodoList::appendTodo(Todo* todo)
{
    m_todos->append( todo );
    connect( todo, SIGNAL(changed()), this, SLOT(todoParentChanged()) );
}

void TodoList::todoParentChanged()
{
    Todo* todo = qobject_cast< Todo* >( sender() );
    if ( todo ) {
        m_todos->notifyObjectChanged( todo );
    }
}

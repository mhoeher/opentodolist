/*
 *  OpenTodoListCore - Core functionality for the OpenTodoList application
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

#include "abstracttodolist.h"

#include "abstracttodo.h"

AbstractTodoList::AbstractTodoList(const QString& key, const QString& type, QObject* parent) :
    QObject(parent),
    m_todos( new TodoList( this ) ),
    m_topLevelTodos( new TodoSortFilterModel( this ) ),
    m_deletedTodos( new TodoSortFilterModel( this ) ),
    m_type( type ),
    m_key( key )
{
    m_topLevelTodos->setFilterMode( TodoSortFilterModel::TodoListEntries | TodoSortFilterModel::HideDeleted );
    m_topLevelTodos->setSortMode( TodoSortFilterModel::PrioritySort );
    m_topLevelTodos->sort( 0 );
    m_topLevelTodos->setSourceModel( m_todos );

    m_deletedTodos->setFilterMode( TodoSortFilterModel::HideNonDeleted );
    m_topLevelTodos->sort( 0 );
    m_topLevelTodos->setSourceModel( m_todos );
    
    connect( this, SIGNAL(nameChanged()), this, SIGNAL(changed()) );
}

AbstractTodoList::TodoList* AbstractTodoList::todos() const
{
    return m_todos;
}

TodoSortFilterModel* AbstractTodoList::topLevelTodos() const
{
    return m_topLevelTodos;
}

TodoSortFilterModel *AbstractTodoList::deletedTodos() const
{
    return m_deletedTodos;
}

QObject *AbstractTodoList::addTodo()
{
    AbstractTodo* todo = new AbstractTodo( this );
    appendTodo( todo );
    return todo;
}

QObject* AbstractTodoList::addTodo(const QString& title, QObject* parentTodo)
{
    AbstractTodo* result = qobject_cast< AbstractTodo* >( addTodo() );
    if ( result ) {
        result->setTitle( title );
        result->setParentTodo( static_cast< AbstractTodo* >( parentTodo ) );
    }
    return result;
}

const QString& AbstractTodoList::name() const
{
    return m_name;
}

void AbstractTodoList::setName(const QString& name)
{
    m_name = name;
    emit nameChanged();
}

void AbstractTodoList::appendTodo(AbstractTodo* todo)
{
    m_todos->append( todo );
    connect( todo, SIGNAL(changed()), this, SLOT(todoParentChanged()) );
}

void AbstractTodoList::todoParentChanged()
{
    AbstractTodo* todo = qobject_cast< AbstractTodo* >( sender() );
    if ( todo ) {
        m_todos->notifyObjectChanged( todo );
    }
}

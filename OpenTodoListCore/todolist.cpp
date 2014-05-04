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
#include "todolistlibrary.h"


/**
   @brief Creates a new invalid TodoList

   This will create a new invalid todo list.

   @sa isNull
 */
TodoList::TodoList(QObject *parent) :
    QObject( parent ),
    m_isNull( true ),
    m_backend(),
    m_struct(),
    m_library( 0 ),
    m_disablePersisting( false )
{
}

/**
   @brief Constructor

   Creates a new TodoList object. The object is created for the
   @p backend and using the information from the @p list structure. The
   @p library is used to interact with the application database. The list
   will be made a child of @p parent.
 */
TodoList::TodoList(const QString &backend,
                   const TodoListStruct &list,
                   TodoListLibrary *library, QObject *parent) :
    QObject( parent ),
    m_isNull( false ),
    m_backend( backend ),
    m_struct( list ),
    m_library( library ),
    m_disablePersisting( false )
{
    Q_ASSERT( library != 0 );
    connect( this, SIGNAL(nameChanged()), this, SIGNAL(changed()) );

    connect( this, SIGNAL(changed()), this, SLOT(persist()) );

    connect( m_library->storage(), SIGNAL(todoListInserted(QString,TodoListStruct)),
             this, SLOT(handleTodoListUpdated(QString,TodoListStruct)) );
    connect( m_library->storage(), SIGNAL(todoListRemoved(QString,TodoListStruct)),
             this, SLOT(handleTodoListRemoved(QString,TodoListStruct)) );
}

/**
   @brief Destructor
 */
TodoList::~TodoList()
{

}

/**
   @brief The ID of the backend this todo list belongs to
 */
QString TodoList::backend() const
{
    return m_backend;
}

/**
   @brief The ID of the todo list
 */
QString TodoList::id() const
{
    return m_struct.id.toString();
}

/**
   @brief The name of the todo list
 */
QString TodoList::name() const
{
    return m_struct.name;
}

/**
   @brief Sets the name of the todo list
 */
void TodoList::setName(const QString &name)
{
    if ( m_struct.name != name ) {
        m_struct.name = name;
        emit nameChanged();
    }
}

/**
   @brief The library the todo list belong to
 */
TodoListLibrary *TodoList::library() const
{
    return m_library.data();
}

/**
   @brief Is the todo list valid?

   If the todo list has been created via the default constructor (which creates
   empty todo lists) this will return true. If the todo list has been created
   by the other constructor, it is valid and isNull is false.
 */
bool TodoList::isNull() const
{
    return m_isNull;
}

void TodoList::handleTodoListUpdated(const QString &backend, const TodoListStruct &list)
{
    if ( backend == m_backend && list.id == m_struct.id ) {
        m_disablePersisting = true;
        setName( list.name );
        m_struct.meta = list.meta;
        m_disablePersisting = false;
    }
}

void TodoList::handleTodoListRemoved(const QString &backend, const TodoListStruct &list)
{
    if ( backend == m_backend && list.id == m_struct.id ) {
        deleteLater();
    }
}

void TodoList::persist()
{
    if ( !m_disablePersisting && m_library ) {
        m_library->storage()->insertTodoList( m_backend, m_struct );
        m_library->notifyTodoListChanged( m_backend, m_struct );
    }
}



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

#include "todo.h"

#include "todolistlibrary.h"


/**
   @brief Creates a new invalid Todo

   This creates a new Todo which is invalid.

   @sa isNull()
 */
Todo::Todo(QObject *parent) :
    QObject( parent ),
    m_isNull( true ),
    m_backend(),
    m_struct(),
    m_library( 0 )
{
}

/**
   @brief Constructor

   Creates a new todo object for the @p backend wrapping around the
   @p todo struct. The @p library will be used to querying
   data from the application database. The object will be made a child
   of the @p parent.
 */
Todo::Todo(const QString &backend,
           const TodoStruct &todo,
           TodoListLibrary *library,
           QObject *parent = 0) :
    QObject( parent ),
    m_isNull( false ),
    m_backend( backend ),
    m_struct( todo ),
    m_library( library )
{
    Q_ASSERT( library != 0 );
    connect( this, SIGNAL(weightChanged()), this, SIGNAL(changed()) );
    connect( this, SIGNAL(progressChanged()), this, SIGNAL(changed()) );
    connect( this, SIGNAL(priorityChanged()), this, SIGNAL(changed()) );
    connect( this, SIGNAL(dueDateChanged()), this, SIGNAL(changed()) );
    connect( this, SIGNAL(titleChanged()), this, SIGNAL(changed()) );
    connect( this, SIGNAL(descriptionChanged()), this, SIGNAL(changed()) );
    connect( this, SIGNAL(deletedChanged()), this, SIGNAL(changed()) );
}

/**
   @brief Destructor
 */
Todo::~Todo()
{
}

/**
   @brief The backend ID the todo belongs to
 */
QString Todo::backend() const
{
    return m_backend;
}

/**
   @brief The unique ID of the todo
 */
QString Todo::id() const
{
    return m_struct.id.toString();
}

/**
   @brief The weight of the todo
 */
double Todo::weight() const
{
    return m_struct.weight;
}

/**
   @brief Sets the weight of the todo
 */
void Todo::setWeight(double weight)
{
    m_struct.weight = weight;
    emit weightChanged();
}

/**
   @brief The progress of the todo
 */
int Todo::progress() const
{
    return qBound( 0, m_struct.progress, 100 );
}

/**
   @brief Sets the progress of the todo
 */
void Todo::setProgress(int progress)
{
    m_struct.progress = qBound( 0, progress, 100 );
    emit progressChanged();
}

/**
   @brief The priority of the todo (from -1 to 10)
 */
int Todo::priority() const
{
    return qBound( -1, m_struct.priority, 10 );
}

/**
   @brief Sets the priority of the todo.
 */
void Todo::setPriority(int priority)
{
    m_struct.priority = qBound( -1, priority, 10 );
    emit priorityChanged();
}

/**
   @brief The ID of the parent todo
 */
QString Todo::parentTodoId() const
{
    return m_struct.parentTodoId.toString();
}

/**
   @brief The ID of the todo list
 */
QString Todo::todoListId() const
{
    return m_struct.todoListId.toString();
}

/**
   @brief The due date of the todo
 */
QDateTime Todo::dueDate() const
{
    return m_struct.dueDate;
}

/**
   @brief Sets the due date of the todo
 */
void Todo::setDueDate(const QDateTime &dueDate)
{
    m_struct.dueDate = dueDate;
    emit dueDateChanged();
}

/**
   @brief The title of the todo
 */
QString Todo::title() const
{
    return m_struct.title;
}

/**
   @brief Sets the title of the todo
 */
void Todo::setTitle(const QString &title)
{
    m_struct.title = title;
    emit titleChanged();
}

/**
   @brief The description of the todo
 */
QString Todo::description() const
{
    return m_struct.description;
}

/**
   @brief Sets the description of the todo
 */
void Todo::setDescription(const QString &description)
{
    m_struct.description = description;
    emit descriptionChanged();
}

/**
   @brief Returns whether the todo is flagged as deleted
 */
bool Todo::isDeleted() const
{
    return m_struct.deleted;
}

/**
   @brief Sets the deleted flag of the todo
 */
void Todo::setDeleted(bool deleted)
{
    m_struct.deleted = deleted;
    emit deletedChanged();
}

/**
   @brief The library the todo belongs to
 */
TodoListLibrary *Todo::library() const
{
    return m_library;
}

/**
   @brief Is the todo valid or "empty"

   Returns false if the todo is valid. If the todo has been created
   via the default constructor, this returns true.
 */
bool Todo::isNull() const
{
    return m_isNull;
}


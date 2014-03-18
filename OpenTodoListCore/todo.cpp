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

#include "todolist.h"
#include "todosortfiltermodel.h"

#include <QDebug>
#include <QQueue>
#include <QSet>

/**
 * @class AbstractTodo
 * @brief Base class for all todos
 *
 * This is the base class for all todos. Depending on the platform, this
 * class get's subclassed to provide e.g. platform specific storage.
 */

/**
 * @brief Constructor
 * @param parent The parent object to use for the todo
 */
Todo::Todo(QUuid id, TodoList *parent) :
    QObject(parent),
    m_id( id ),
    m_title( QString() ),
    m_description( QString() ),
    m_progress( 0 ),
    m_priority( -1 ),
    m_parentTodo( 0 ),
    m_deleted( false ),
    m_dueDate( QDateTime() ),
    m_lastProgress( -1 ),
    m_subTodosModel( new TodoSortFilterModel( this ) )
{
    m_subTodosModel->setParentTodo( this );
    m_subTodosModel->setSourceModel( parent->todos() );
    m_subTodosModel->setFilterMode( TodoSortFilterModel::SubTodos | TodoSortFilterModel::HideDeleted );
    m_subTodosModel->sort( 0 );
    
    connect( m_subTodosModel, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)),
             this, SLOT(childDataChanged()) );

    connect( m_subTodosModel, SIGNAL(itemCountChanged()), this, SIGNAL(numTodosChanged()) );
    connect( m_subTodosModel, SIGNAL(itemCountChanged()), this, SIGNAL(numOpenTodosChanged()) );
    
    connect( this, SIGNAL(idChanged()), this, SIGNAL(changed()) );
    connect( this, SIGNAL(titleChanged()), this, SIGNAL(changed()) );
    connect( this, SIGNAL(descriptionChanged()), this, SIGNAL(changed()) );
    connect( this, SIGNAL(progressChanged()), this, SIGNAL(changed()) );
    connect( this, SIGNAL(priorityChanged()), this, SIGNAL(changed()) );
    connect( this, SIGNAL(parentTodoChanged()), this, SIGNAL(changed()) );
    connect( this, SIGNAL(deletedChanged()), this, SIGNAL(changed()) );
    connect( this, SIGNAL(dueDateChanged()), this, SIGNAL(changed()));
}

/**
   @brief The globally unique ID of the todo

   A globally unique ID that can be used to identify a todo even across
   networks.
 */
QUuid Todo::id() const
{
    return m_id;
}

int Todo::priority() const
{
    return m_priority;
}

void Todo::setPriority(int priority)
{
    m_priority = qBound( -1, priority, 10 );
    emit priorityChanged();
}

int Todo::progress() const
{
    if ( m_subTodosModel->rowCount() > 0 ) {
        int result = 0;
        for ( int i = 0; i < m_subTodosModel->rowCount(); ++i ) {
            Todo* subTodo = qobject_cast< Todo* >(
                        m_subTodosModel->index( i, 0 ).data(
                            TodoSortFilterModel::TodoModel::ObjectRole ).value< QObject* >() );
            result += subTodo->progress();
        }
        return result / m_subTodosModel->rowCount();
    } else {
        return m_progress;
    }
}

void Todo::setProgress(int progress)
{
    m_lastProgress = m_progress;
    m_progress = qBound( 0, progress, 100 );
    emit progressChanged();
}

QString Todo::description() const
{
    return m_description;
}

void Todo::setDescription(const QString &description)
{
    m_description = description;
    emit descriptionChanged();
}

QString Todo::title() const
{
    return m_title;
}

void Todo::setTitle(const QString &title)
{
    m_title = title;
    emit titleChanged();
}

Todo* Todo::parentTodo() const
{
    return m_parentTodo;
}

void Todo::setParentTodo(QObject* parentTodo)
{
    // parent todo must be null or we must have common todo list
    if ( !parentTodo || parentTodo->parent() == parent() ) {
        // Check for cycles...
        if ( canMoveTo( parentTodo ) ) {
            if ( parentTodo != m_parentTodo ) {
                m_parentTodo = static_cast< Todo* >( parentTodo );
                emit parentTodoChanged();
            }
        }
    }
}

bool Todo::isDeleted() const
{
    return m_deleted;
}

void Todo::setDeleted(bool deleted)
{
    m_deleted = deleted;
    emit deletedChanged();
}

QDateTime Todo::dueDate() const
{
    return m_dueDate;
}

void Todo::setDueDate(const QDateTime &dateTime)
{
    m_dueDate = dateTime;
    emit dueDateChanged();
}

TodoList* Todo::parent() const
{
    return qobject_cast< TodoList* >( QObject::parent() );
}

/**
   @brief Returns the number of sub todos

   Returns the number of sub todos of this todo. Deleted todos are not
   included in this.
 */
int Todo::numTodos() const
{
    int result = 0;
    for ( int i = 0; i < m_subTodosModel->rowCount(); ++i ) {
        Todo* todo = qobject_cast< Todo* >(
                    m_subTodosModel->index( i, 0 ).data( ObjectModel<Todo>::ObjectRole )
                    .value< QObject* >() );
        if ( todo && !todo->isDeleted() ) {
            ++result;
        }
    }
    return result;
}

/**
   @brief Returns the number of open sub todos

   This returns the number of open sub todos of this todo. Deleted todos
   are not included in this.
 */
int Todo::numOpenTodos() const
{
    int result = 0;
    for ( int i = 0; i < m_subTodosModel->rowCount(); ++i ) {
        Todo* todo = qobject_cast< Todo* >(
                    m_subTodosModel->index( i, 0 ).data( ObjectModel<Todo>::ObjectRole )
                    .value< QObject* >() );
        if ( todo && !todo->isDeleted() && !todo->isCompleted() ) {
            ++result;
        }
    }
    return result;
}

/**
   @brief Check if the todo can be moved to another parent.

   This method can be used to check whether this todo can be made a child
   todo of the @p newParent. If this is the case, true is returned; otherwise,
   false. This method is used internally when setting the parent of a todo
   to ensure that no cyclic parent chains are built.

 */
bool Todo::canMoveTo(QObject *newParent) const
{
    QSet< const Todo* > allChildren;
    QQueue< const Todo* > queue;
    queue.enqueue( this );
    allChildren.insert( this );
    while ( !queue.isEmpty() ) {
        const Todo *nextTodo = queue.dequeue();
        if ( nextTodo == newParent ) {
            // the new parent is a child item of the current one - cannot move!
            return false;
        }
        for ( int i = 0; i < nextTodo->subTodos()->rowCount(); ++i ) {
            Todo *todo = qobject_cast< Todo* >(
                        nextTodo->subTodos()->index( i, 0 ).data(
                        TodoSortFilterModel::TodoModel::ObjectRole )
                    .value< QObject* >() );
            if ( todo && !allChildren.contains( todo ) ) {
                allChildren.insert( todo );
                queue.enqueue( todo );
            }
        }
    }
    return true;
}

/**
   @brief Clones properties from another todo

   Copies over attributes from @p otherTodo. The todo copied from will
   be scheduled for deletion afterwards, so after calling this, do not
   access the other todo anymore.
 */
void Todo::cloneFrom(QObject *otherTodo)
{
    if ( otherTodo ) {
        Todo *asTodo = qobject_cast< Todo* >( otherTodo );
        if ( asTodo ) {
            m_deleted = asTodo->m_deleted;
            m_description = asTodo->m_description;
            m_dueDate = asTodo->m_dueDate;
            m_id = asTodo->m_id;
            m_lastProgress = asTodo->m_lastProgress;
            m_priority = asTodo->m_priority;
            m_progress = asTodo->m_progress;
            m_title = asTodo->m_title;
            emit changed();
            asTodo->dispose();
        } else {
            qWarning() << otherTodo << "is not a AbstractTodo";
        }
    }
}

void Todo::childDataChanged()
{
    // emulate a change of progress to propagate any changes up the tree
    if ( m_subTodosModel->rowCount() > 0 ) {
        emit progressChanged();
    }
    emit numOpenTodosChanged();
}

void Todo::toggleCompleted(int newProgress) {
    if ( newProgress >= 0 ) {
        // set to new progress regardless of our current state
        setProgress( newProgress );
    } else {
        // toggle
        if ( m_progress == 100 ) {
            setProgress( m_lastProgress < 0 ? 0 : m_lastProgress );
        } else {
            setProgress( 100 );
        }
    }
}

/**
   @brief Deletes the todo

   This will delete the todo. Internally, the following actions will be
   done:

   <ol>
   <li>For each child todo, call its dispose() method.</li>
   <li>The aboutToBeDisposed() signal is emitted to let other objects know what
       we are planning to do.</li>
   <li>The destroy() method of the todo is called to do any cleanup. Sub-classes
       ob AbstractTodo should implement this method to whatever might be
       required.</li>
   <li>Finally, we are calling deleteLater() to schedule deletion of the todo
       for when we are returning to the event loop.</li>
   </ul>
 */
void Todo::dispose()
{
    QList< Todo* > todos;
    for ( int i = 0; i < m_subTodosModel->rowCount(); ++i ) {
        Todo* todo = qobject_cast< Todo* >(
                    m_subTodosModel->index( i, 0 ).data(
                        TodoSortFilterModel::TodoModel::ObjectRole ).value< QObject* >() );
        if ( todo ) {
            todos.append( todo );
        }
    }
    foreach (  Todo* todo, todos ) {
        todo->dispose();
    }
    emit aboutToBeDisposed( this );
    destroy();
    deleteLater();
}

/**
   @brief Overrides the ID of the todo

   This overrides the @p id of the todo. Note that you do not usually want to use
   this method as it internally changes the identity of the todo. In fact, the
   only real use case is when the todo is restored when restarting the
   application and there is no "sane" way for the todo list to
   know the ID upfront the todo is created and can read the ID back on it's own
   (e.g. from external memory or the network).
 */
void Todo::setId(QUuid id)
{
    m_id = id;
    emit idChanged();
}

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
#include "todoliststoragequery.h"

#include <QDebug>


/**
   @brief Creates a new invalid Todo

   This creates a new Todo which is invalid.

   @sa isNull()
 */
Todo::Todo(QObject *parent) :
    QObject( parent ),
    m_isNull( true ),
    m_backend(),
    m_struct( BackendWrapper::NullTodo ),
    m_todoList( BackendWrapper::NullTodoList ),
    m_library( 0 ),
    m_previousProgress( -1 ),
    m_disablePersisting( false )
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
    m_todoList( BackendWrapper::NullTodoList ),
    m_library( library ),
    m_previousProgress( -1 ),
    m_disablePersisting( false )
{
    setupTodo();
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
    if ( m_struct.weight != weight ) {
        m_struct.weight = weight;
        emit weightChanged();
    }
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
    if ( progress != m_struct.progress ) {
        m_previousProgress = m_struct.progress;
        m_struct.progress = qBound( 0, progress, 100 );
        emit progressChanged();
    }
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
    if ( m_struct.priority != priority ) {
        m_struct.priority = qBound( -1, priority, 10 );
        emit priorityChanged();
    }
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
    if ( m_struct.dueDate != dueDate ) {
        m_struct.dueDate = dueDate;
        emit dueDateChanged();
    }
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
    if ( m_struct.title != title ) {
        m_struct.title = title;
        emit titleChanged();
    }
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
    if ( m_struct.description != description ) {
        m_struct.description = description;
        emit descriptionChanged();
    }
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
    if ( m_struct.deleted != deleted ) {
        m_struct.deleted = deleted;
        emit deletedChanged();
    }
}

/**
   @brief The library the todo belongs to
 */
TodoListLibrary *Todo::library() const
{
    return m_library.data();
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

/**
   @brief Is the todo completed?

   Returns true if the todo is completed (i.e. the progress is 100%) or
   false otherwise.
 */
bool Todo::isDone() const
{
    return m_struct.progress >= 100;
}

/**
   @brief Can new sub-todos be created within this todo

   This returns true in case new sub-todos can be created within this todo
 */
bool Todo::canCreateTodos() const
{
    if ( m_todoList.id.isNull() ) {
        return false;
    }
    if ( m_library ) {
        return m_library->canAddTodo( m_backend, m_todoList, m_struct );
    }
    return false;
}

/**
   @brief Mimics another todo

   This method can be used to "clone" a todo: First, create a new null-Todo using
   the default constructor. Then, call shadow() with a valid other todo to mimic it.

   @note You can shadow() only once: Calling it on a non-null todo has no effect.
 */
void Todo::shadow(Todo *other)
{
    if ( m_isNull && other ) {
        m_isNull = other->m_isNull;
        m_backend = other->m_backend;
        m_struct = other->m_struct;
        m_todoList = other->m_todoList;
        m_library = other->m_library;
        m_previousProgress = other->m_previousProgress;
        if ( !m_isNull ) {
            setupTodo();
        }
        emit reset();
    }
}

/**
   @brief Creates a TodoStruct from a variant map

   This static method takes as an input a list of @p properties and
   puts them into a TodoStruct structure which is returned afterwards.

 */
TodoStruct Todo::fromVariant( const QVariantMap &properties )
{
    TodoStruct result = BackendWrapper::NullTodo;
    result.deleted = properties.value( "deleted", result.deleted ).toBool();
    result.description = properties.value( "description", result.description ).toString();
    result.dueDate = properties.value( "dueDate", result.dueDate ).toDateTime();
    result.parentTodoId = properties.value( "parentTodoId", result.parentTodoId ).toUuid();
    result.priority = properties.value( "priority", result.priority ).toInt();
    result.progress = properties.value( "progress", result.progress ).toInt();
    result.title = properties.value( "title", result.title ).toString();
    result.todoListId = properties.value( "todoListId", result.todoListId ).toUuid();
    result.weight = properties.value( "weight", result.weight ).toDouble();
    return result;
}

/**
   @brief Toggles the state of the todo between "done" and "not done"

   @sa setProgress
 */
void Todo::toggle()
{
    if ( m_struct.progress == 100 ) {
        setProgress( m_previousProgress >= 0 && m_previousProgress < 100 ?
                         m_previousProgress : 0 );
    } else {
        setProgress( 100 );
    }
}

/**
   @brief Adds a new sub-todo with the given @p properties to this todo
 */
void Todo::addTodo(const QVariantMap &properties )
{
    if ( m_library && !isNull() && !m_todoList.id.isNull() ) {
        TodoStruct newTodo = fromVariant( properties );
        m_library->addTodo( m_backend, newTodo, m_todoList, m_struct );
    }
}

/**
   @brief Disposes the todo

   Deletes this and any sub-todo (recursively).
 */
void Todo::dispose()
{
    if ( m_library && !isNull() ) {
        RecursiveDeleteTodoQuery *query = new RecursiveDeleteTodoQuery( m_backend, m_struct );
        connect( query, SIGNAL(notifyTodoDeleted(QString,TodoStruct)),
                 m_library.data(), SLOT(notifyTodoDeleted(QString,TodoStruct)),
                 Qt::QueuedConnection );
        m_library->storage()->runQuery( query );
    }
}

void Todo::setupTodo()
{
    Q_ASSERT( m_library != 0 );
    connect( this, SIGNAL(weightChanged()), this, SIGNAL(changed()) );
    connect( this, SIGNAL(progressChanged()), this, SIGNAL(changed()) );
    connect( this, SIGNAL(priorityChanged()), this, SIGNAL(changed()) );
    connect( this, SIGNAL(dueDateChanged()), this, SIGNAL(changed()) );
    connect( this, SIGNAL(titleChanged()), this, SIGNAL(changed()) );
    connect( this, SIGNAL(descriptionChanged()), this, SIGNAL(changed()) );
    connect( this, SIGNAL(deletedChanged()), this, SIGNAL(changed()) );

    connect( this, SIGNAL(reset()), this, SIGNAL(weightChanged()) );
    connect( this, SIGNAL(reset()), this, SIGNAL(progressChanged()) );
    connect( this, SIGNAL(reset()), this, SIGNAL(priorityChanged()) );
    connect( this, SIGNAL(reset()), this, SIGNAL(dueDateChanged()) );
    connect( this, SIGNAL(reset()), this, SIGNAL(titleChanged()) );
    connect( this, SIGNAL(reset()), this, SIGNAL(descriptionChanged()) );
    connect( this, SIGNAL(reset()), this, SIGNAL(deletedChanged()) );
    connect( this, SIGNAL(reset()), this, SIGNAL(canCreateTodosChanged()) );

    connect( this, SIGNAL(changed()), this, SLOT(persist()) );
    connect( m_library->storage(), SIGNAL(todoInserted(QString,TodoStruct)),
            this, SLOT(handleTodoUpdates(QString,TodoStruct)) );
    connect( m_library->storage(), SIGNAL(todoRemoved(QString,TodoStruct)),
            this, SLOT(handleTodoRemoved(QString,TodoStruct)) );
    connect( m_library->storage(), SIGNAL(todoListInserted(QString,TodoListStruct)),
            this, SLOT(handleTodoListUpdated(QString,TodoListStruct)) );

    if ( m_library ) {
        TodoListByIdQuery *query = new TodoListByIdQuery( m_backend, m_struct.todoListId.toString() );
        connect( query, SIGNAL(todoListAvailable(QString,TodoListStruct)),
                 this, SLOT(handleTodoListAvailable(QString,TodoListStruct)),
                 Qt::QueuedConnection );
        m_library->storage()->runQuery( query );
    }
}

/**
   @brief Saves back any changes of the todo into the
 */
void Todo::persist()
{
    if ( !m_disablePersisting && m_library ) {
        m_library->storage()->insertTodo( m_backend, m_struct );
        m_library->notifyTodoChanged( m_backend, m_struct );
    }
}

void Todo::handleTodoUpdates(const QString &backend, const TodoStruct &todo)
{
    if ( backend == m_backend && todo.id == m_struct.id ) {
        m_disablePersisting = true;
        setWeight( todo.weight );
        setProgress( todo.progress );
        setPriority( todo.priority );
        setDueDate( todo.dueDate );
        setTitle( todo.title );
        setDescription( todo.description );
        setDeleted( todo.deleted );
        m_disablePersisting = false;
    }
}

void Todo::handleTodoRemoved(const QString &backend, const TodoStruct &todo)
{
    if ( backend == m_backend && todo.id == m_struct.id ) {
        deleteLater();
    }
}

void Todo::handleTodoListUpdated(const QString &backend, const TodoListStruct &list)
{
    if ( backend == m_backend && list.id == m_todoList.id ) {
        m_todoList = list;
        emit canCreateTodosChanged();
    }
}

void Todo::handleTodoListAvailable(const QString &backend, const TodoListStruct &list)
{
    if ( backend == m_backend ) {
        m_todoList = list;
        emit canCreateTodosChanged();
    }
}



Todo::Comparator::Comparator(Todo::TodoSortMode sortMode) :
    m_sortMode( sortMode )
{

}

int Todo::Comparator::operator ()(Todo * const &first, Todo * const &second) const
{
    if ( first && second ) {
        // always show deleted todos after non-deleted ones
        if ( first->isDeleted() && !second->isDeleted() ) {
            return 1;
        }
        if ( second->isDeleted() && !first->isDeleted() ) {
            return -1;
        }

        // always show open todos before done ones:
        if ( first->isDone() && !second->isDone() ) {
            return 1;
        }
        if ( second->isDone() && !first->isDone() ) {
            return -1;
        }

        // sort depending on mode
        switch ( m_sortMode ) {

        // "default" sorting ;) Applied everywhere
        case SortTodoByName:
        //    return first->title().localeAwareCompare( second->title() ) <= 0;
            break;

        case SortTodoByPriority:
            if ( first->priority() != second->priority() ) {
                return second->priority() - first->priority();
            }
            break;

        case SortTodoByPercentageDone:
            if ( first->progress() != second->progress() ) {
                return second->progress() - first->progress();
            }
            break;

        case SortTodoByDueDate:
            if ( first->dueDate() != second->dueDate() ) {
                if ( !first->dueDate().isValid() ) {
                    return 1;
                }
                if ( !second->dueDate().isValid() ) {
                    return -1;
                }
                return first->dueDate() < second->dueDate() ? -1 : 1;
            }
            break;
        }

        // compare everything else by title
        return first->title().localeAwareCompare( second->title() );
    }
    return 0;
}

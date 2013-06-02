#include "abstracttodo.h"

#include "abstracttodolist.h"
#include "todosortfiltermodel.h"

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
AbstractTodo::AbstractTodo(AbstractTodoList *parent) :
QObject(parent),
    m_title( QString() ),
    m_description( QString() ),
    m_progress( 0 ),
    m_priority( -1 ),
    m_parentTodo( 0 ),
    m_subTodosModel( new TodoSortFilterModel( this ) )
{
    m_subTodosModel->setParentTodo( this );
    m_subTodosModel->setSourceModel( parent->todos() );
    m_subTodosModel->setFilterMode( TodoSortFilterModel::SubTodos );
    
    connect( this, SIGNAL(titleChanged()), this, SIGNAL(changed()) );
    connect( this, SIGNAL(descriptionChanged()), this, SIGNAL(changed()) );
    connect( this, SIGNAL(progressChanged()), this, SIGNAL(changed()) );
    connect( this, SIGNAL(priorityChanged()), this, SIGNAL(changed()) );
    connect( this, SIGNAL(parentTodoChanged()), this, SIGNAL(changed()) );
}

int AbstractTodo::priority() const
{
    return m_priority;
}

void AbstractTodo::setPriority(int priority)
{
    m_priority = qBound( -1, priority, 10 );
    emit priorityChanged();
}

int AbstractTodo::progress() const
{
    return m_progress;
}

void AbstractTodo::setProgress(int progress)
{
    m_progress = qBound( 0, progress, 100 );
    emit progressChanged();
}

QString AbstractTodo::description() const
{
    return m_description;
}

void AbstractTodo::setDescription(const QString &description)
{
    m_description = description;
    emit descriptionChanged();
}

QString AbstractTodo::title() const
{
    return m_title;
}

void AbstractTodo::setTitle(const QString &title)
{
    m_title = title;
    emit titleChanged();
}

AbstractTodo* AbstractTodo::parentTodo() const
{
    return m_parentTodo;
}

void AbstractTodo::setParentTodo(QObject* parentTodo)
{
    if ( !parentTodo || parentTodo->parent() == parent() ) {
        AbstractTodo* p = static_cast< AbstractTodo* >( parentTodo );
        while ( p && p != this ) {
            p = p->parentTodo();
        }
        if ( p != this ) {
            m_parentTodo = static_cast< AbstractTodo* >( parentTodo );
            emit parentTodoChanged();
        }
    }
}

AbstractTodoList* AbstractTodo::parent() const
{
    return qobject_cast< AbstractTodoList* >( QObject::parent() );
}

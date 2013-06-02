#include "abstracttodolist.h"

#include "abstracttodo.h"

AbstractTodoList::AbstractTodoList(const QString& key, const QString& type, QObject* parent) :
    QObject(parent),
    m_todos( new TodoList( this ) ),
    m_topLevelTodos( new TodoSortFilterModel( this ) ),
    m_type( type ),
    m_key( key )
{
    m_topLevelTodos->setFilterMode( TodoSortFilterModel::TodoListEntries );
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
    connect( todo, SIGNAL(parentTodoChanged()), this, SLOT(todoParentChanged()) );
}

void AbstractTodoList::todoParentChanged()
{
    AbstractTodo* todo = qobject_cast< AbstractTodo* >( sender() );
    if ( todo ) {
        m_todos->notifyObjectChanged( todo );
    }
}

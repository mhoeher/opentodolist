#include "todomodel.h"

#include "todolistlibrary.h"

TodoModel::TodoModel(QObject *parent) :
    QAbstractListModel(parent),
    m_queryType( InvalidQuery ),
    m_todos(),
    m_library( 0 ),
    m_loadedTodos(),
    m_newLoadedTodos(),
    m_todoList( 0 ),
    m_parentTodo( 0 )
{
    connect( this, SIGNAL(todoListChanged()), this, SIGNAL(changed()) );
    connect( this, SIGNAL(parentTodoChanged()), this, SIGNAL(changed()) );
    connect( this, SIGNAL(rowsInserted(QModelIndex,int,int)),
             this, SIGNAL(countChanged()) );
    connect( this, SIGNAL(rowsRemoved(QModelIndex,int,int)),
             this, SIGNAL(countChanged()) );
}

TodoModel::~TodoModel()
{
}

TodoListLibrary *TodoModel::library() const
{
    return m_library.data();
}

void TodoModel::setLibrary(TodoListLibrary *library)
{
    m_library = library;
    emit libraryChanged();
}

int TodoModel::rowCount(const QModelIndex &parent) const
{
    if ( parent.isValid() ) {
        return 0;
    } else {
        return m_todos.size();
    }
}

QVariant TodoModel::data(const QModelIndex &index, int role) const
{
    if ( index.isValid() && index.row() < m_todos.size() ) {
        if ( role == Qt::DisplayRole ) {
            return QVariant::fromValue< Todo* >(
                        m_todos.at( index.row() ) );
        }
    }
    return QVariant();
}

/**
   @brief The type of content this model displays
 */
TodoModel::QueryType TodoModel::queryType() const
{
    return m_queryType;
}

/**
   @brief Sets what is shown by the model
 */
void TodoModel::setQueryType(const QueryType &queryType)
{
    m_queryType = queryType;
    emit queryTypeChanged();
}

/**
   @brief Updates the model

   This refreshes the model after updating the query properties of the model.
 */
void TodoModel::update()
{
    if ( m_library && m_queryType != InvalidQuery ) {
        TodoStorageQuery *query = new TodoStorageQuery();
        query->setQueryType( m_queryType );
        bool queryIsValid = true;

        switch ( m_queryType ) {
        case QueryTopLevelTodosInTodoList:
            if ( m_todoList ) {
                query->setBackend( m_todoList->backend() );
                query->setTodoListId( m_todoList->id() );
            }
            break;

        case QuerySubTodosOfTodo:
            if ( m_parentTodo ) {
                query->setBackend( m_parentTodo->backend() );
                query->setTodoListId( m_parentTodo->todoListId() );
                query->setParentTodoId( m_parentTodo->id() );
            }
            break;

        default:
            queryIsValid = false;
            break;
        }

        if ( queryIsValid ) {
            connect( query, SIGNAL(todoAvailable(QString,TodoStruct)),
                     this, SLOT(addTodo(QString,TodoStruct)), Qt::QueuedConnection );
            connect( query, SIGNAL(finished()),
                     this, SLOT(removeExtraneousTodos()), Qt::QueuedConnection );
            m_library->storage()->runQuery( query );
        } else {
            delete query;
        }
    }
}
Todo *TodoModel::parentTodo() const
{
    return m_parentTodo.data();
}

void TodoModel::setParentTodo(Todo *parentTodo)
{
    m_parentTodo = parentTodo;
}

TodoList *TodoModel::todoList() const
{
    return m_todoList.data();
}

void TodoModel::setTodoList(TodoList *todoList)
{
    m_todoList = todoList;
}


QString TodoModel::idForTodo(const QString &backend, const QString &todoId)
{
    return backend + "." + todoId;
}

void TodoModel::addTodo(const QString &backend, const TodoStruct &todo)
{
    QString id = idForTodo( backend, todo.id.toString() );
    m_newLoadedTodos.insert( id );
    if ( !m_loadedTodos.contains( id ) ) {
        Todo *newTodo = new Todo( backend, todo, m_library, this );
        if ( newTodo ) {
            emit beginInsertRows( QModelIndex(), rowCount(), rowCount() );
            m_todos << newTodo;
            emit endInsertRows();
            m_loadedTodos.insert( id );
        }
    }
}

void TodoModel::removeExtraneousTodos()
{
    for ( int i = rowCount() - 1; i >= 0; --i ) {
        Todo *todo = m_todos.at( i );
        if ( todo ) {
            QString id = idForTodo( todo->backend(), todo->id() );
            if ( !m_newLoadedTodos.contains( id ) ) {
                emit beginRemoveRows( QModelIndex(), i, i );
                m_todos.remove( i );
                emit endRemoveRows();
                m_loadedTodos.remove( id );
                todo->deleteLater();
            }
        }
    }
    m_newLoadedTodos.clear();
}

TodoStorageQuery::TodoStorageQuery() :
    TodoListStorageQuery()
{
}

TodoStorageQuery::~TodoStorageQuery()
{
}

QString TodoStorageQuery::backend() const
{
    return m_backend;
}

void TodoStorageQuery::setBackend(const QString &backend)
{
    m_backend = backend;
}
QString TodoStorageQuery::todoListId() const
{
    return m_todoListId;
}

void TodoStorageQuery::setTodoListId(const QString &todoListId)
{
    m_todoListId = todoListId;
}
QString TodoStorageQuery::parentTodoId() const
{
    return m_parentTodoId;
}

void TodoStorageQuery::setParentTodoId(const QString &parentTodoId)
{
    m_parentTodoId = parentTodoId;
}

TodoModel::QueryType TodoStorageQuery::queryType() const
{
    return m_queryType;
}

void TodoStorageQuery::setQueryType(const TodoModel::QueryType &queryType)
{
    m_queryType = queryType;
}

void TodoStorageQuery::beginRun()
{
    // nothing to be done here
}

#include <QDebug>
bool TodoStorageQuery::query(QString &query, QVariantMap &args)
{
    switch ( m_queryType ) {
    case TodoModel::QueryTopLevelTodosInTodoList:
        query = "SELECT * FROM todo WHERE parentTodo IS NULL AND todoList=:todoList AND backend=:backend ORDER BY title ASC;";
        args.insert( "todoList", m_todoListId );
        args.insert( "backend", m_backend );
        return true;

    case TodoModel::QuerySubTodosOfTodo:
        query = "SELECT * FROM todo WHERE todoList=:todoList AND parentTodo=:parentTodo AND backend=:backend ORDER BY title ASC;";
        args.insert( "todoList", m_todoListId );
        args.insert( "parentTodo", m_parentTodoId );
        args.insert( "backend", m_backend );
        qDebug() << query << args;
        return true;

    default:
        return false;
    }
}

void TodoStorageQuery::recordAvailable(const QVariantMap &record)
{
    TodoStruct todo = todoFromRecord( record );
    emit todoAvailable( record.value( "backend" ).toString(), todo );
}

void TodoStorageQuery::endRun()
{
    emit finished();
}





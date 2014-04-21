#include "todomodel.h"

#include "todolistlibrary.h"

TodoModel::TodoModel(QObject *parent) :
    QAbstractListModel(parent),
    m_todos(),
    m_library( 0 ),
    m_loadedTodos(),
    m_newLoadedTodos(),
    m_backend(),
    m_todoListId(),
    m_parentTodoId()
{
    connect( this, SIGNAL(backendChanged()), this, SIGNAL(changed()) );
    connect( this, SIGNAL(todoListIdChanged()), this, SIGNAL(changed()) );
    connect( this, SIGNAL(parentTodoIdChanged()), this, SIGNAL(changed()) );

    connect( this, SIGNAL(changed()), this, SLOT(update()) );
}

TodoModel::~TodoModel()
{
}

TodoListLibrary *TodoModel::library() const
{
    return m_library;
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
QString TodoModel::backend() const
{
    return m_backend;
}

void TodoModel::setBackend(const QString &backend)
{
    if ( m_backend != backend ) {
        m_backend = backend;
        emit backendChanged();
    }
}
QString TodoModel::todoListId() const
{
    return m_todoListId;
}

void TodoModel::setTodoListId(const QString &todoListId)
{
    if ( m_todoListId != todoListId ) {
        m_todoListId = todoListId;
        emit todoListIdChanged();
    }
}

QString TodoModel::parentTodoId() const
{
    return m_parentTodoId;
}

void TodoModel::setParentTodoId(const QString &parentTodoId)
{
    if ( m_parentTodoId != parentTodoId ) {
        m_parentTodoId = parentTodoId;
        emit parentTodoIdChanged();
    }
}

void TodoModel::update()
{
    if ( m_library ) {
        TodoStorageQuery *query = new TodoStorageQuery();
        query->setBackend( m_backend );
        query->setTodoListId( m_todoListId );
        query->setParentTodoId( m_parentTodoId );
        connect( query, SIGNAL(todoAvailable(QString,TodoStruct)),
                 this, SLOT(addTodo(QString,TodoStruct)), Qt::QueuedConnection );
        connect( query, SIGNAL(finished()),
                 this, SLOT(removeExtraneousTodos()), Qt::QueuedConnection );
        m_library->storage()->runQuery( query );
    }
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

void TodoStorageQuery::beginRun()
{
    // nothing to be done here
}

bool TodoStorageQuery::query(QString &query, QVariantMap &args)
{
    QStringList conditions;
    if ( !( m_backend.isEmpty() || m_todoListId.isEmpty() ) ) {
        conditions.append( "backend=:backend" );
        args.insert( "backend", m_backend );
        conditions.append( "todoList=:todoList" );
        args.insert( "todoList", m_todoListId );
    }
    if ( !m_parentTodoId.isEmpty() ) {
        conditions.append( "parentTodo=:parentTodo" );
        args.insert( "parentTodo", m_parentTodoId );
    }
    if ( conditions.isEmpty() ) {
        return false;
    } else {
        query = QString( "SELECT * FROM todo WHERE %1;" ).arg(
                    conditions.join( " AND " ) );
        return true;
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



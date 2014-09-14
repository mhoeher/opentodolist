#include "todoliststoragequery.h"

#include "todoliststorage.h"

#include <QJsonDocument>

/**
   @brief Constructor
 */
TodoListStorageQuery::TodoListStorageQuery(QObject *parent) :
    QObject(parent)
{
}

/**
   @brief Destructor
 */
TodoListStorageQuery::~TodoListStorageQuery()
{

}

/**
   @brief The query is to be executed

   This method is called when the query is to be executed.
 */
void TodoListStorageQuery::beginRun()
{
    // nothing to be done here
}

/**
   @brief The query to be run

   This method is called to get the query that is to be run. Sub-classes
   should write the SQL query into @p query. If the query contains any
   user specified values, they can be put into the @p args map. When executing
   the query, the args will be inserted into the query string in a safe way.

   @return True in case everything is okay. In that case, the query will next
     be run on the database. If this returns false, nothing happens and
     execution will stop (but endRun() will still be called.
 */
bool TodoListStorageQuery::query(QString &query, QVariantMap &args)
{
    Q_UNUSED( query );
    Q_UNUSED( args );
    return false;
}

/**
   @brief A new record is available

   This is called for each record that is read from the database when
   running the query.

   The values read are passed in via the @p record map.
 */
void TodoListStorageQuery::recordAvailable(const QVariantMap &record)
{
    Q_UNUSED( record );
}

/**
   @brief Running the query has finished

   This method is called when running the query is done.
 */
void TodoListStorageQuery::endRun()
{

}

/**
   @brief Is the query finished?

   This method can be used to implement a multi-step query. The default implementation
   returns false, that means the sequence of method calls is:

   1. beginRun()
   2. query()
   3. recordAvailable() (for every record that might be returned)
   4. endRun()

   Sometimes it might be required to continue with another query after a
   first successful one. For this, you can re-implement this method and return true
   as long as you have another query to be made. The sequence of calls would then change
   to:

   1. beginRun()
   2. query()
   3. recordAvailable() (for every record that might be returned)
   4. endRun()
   5. if hasNext() goto 1

 */
bool TodoListStorageQuery::hasNext() const
{
    return false;
}

/**
   @brief Helper method: Create a TodoListStruct from a @p record from the DB
 */
TodoListStruct TodoListStorageQuery::todoListFromRecord(const QVariantMap &record)
{
    TodoListStruct result;
    result.id = record.value( "id", QUuid().toString() ).toString();
    result.meta = QJsonDocument::fromJson( record.value( "meta", QByteArray() ).toByteArray() ).toVariant().toMap();
    result.name = record.value( "name", QString() ).toString();
    return result;
}

/**
   @brief Helper method: Create a TodoStruct from a @p record from the DB
 */
TodoStruct TodoListStorageQuery::todoFromRecord(const QVariantMap &record)
{
    TodoStruct result;
    result.deleted = record.value( "deleted", false ).toBool();
    result.description = record.value( "description", QString() ).toString();
    result.dueDate = record.value( "dueDate", QDateTime() ).toDateTime();
    result.id = record.value( "id", QUuid().toString() ).toString();
    result.meta = QJsonDocument::fromJson( record.value( "meta", QByteArray() ).toByteArray() ).toVariant().toMap();
    result.parentTodoId = record.value( "parentTodo", QUuid().toString() ).toString();
    // note: if "null" was put in, the record will contain an empty string for priority
    if ( record.contains( "priority" ) && record.value( "priority" ).toString().isEmpty() ) {
        result.priority = -1;
    } else {
        result.priority = record.value( "priority" ).toInt();
    }
    result.progress = record.value( "progress", 0 ).toInt();
    result.title = record.value( "title", QString() ).toString();
    result.todoListId = record.value( "todoList", QUuid().toString() ).toString();
    result.weight = record.value( "weight", 0.0 ).toDouble();
    return result;
}

/**
   @brief The worker that is processing the query.
 */
TodoListStorageWorker *TodoListStorageQuery::worker() const
{
    return m_worker;
}


TodoListByIdQuery::TodoListByIdQuery(const QString &backend, const QString &todoListId, QObject *parent) :
    TodoListStorageQuery( parent ),
    m_backend( backend ),
    m_todoListId( todoListId )
{
}

TodoListByIdQuery::~TodoListByIdQuery()
{
}

bool TodoListByIdQuery::query(QString &query, QVariantMap &args)
{
    query = "SELECT * FROM todoList WHERE id=:id AND backend=:backend;";
    args.insert( "backend", m_backend );
    args.insert( "id", m_todoListId );
    return true;
}

void TodoListByIdQuery::recordAvailable(const QVariantMap &record)
{
    TodoListStruct list = todoListFromRecord( record );
    emit todoListAvailable( m_backend, list );
}


RecursiveDeleteTodoQuery::RecursiveDeleteTodoQuery(const QString &backend, const TodoStruct &todo, QObject *parent) :
    TodoListStorageQuery( parent ),
    m_backend( backend ),
    m_currentTodo(),
    m_todosToDelete()
{
    m_todosToDelete.enqueue( todo );
}

void RecursiveDeleteTodoQuery::beginRun()
{
    m_currentTodo = m_todosToDelete.dequeue();
    emit notifyTodoDeleted( m_backend, m_currentTodo );
}

bool RecursiveDeleteTodoQuery::query(QString &query, QVariantMap &args)
{
    query = "SELECT * FROM todo WHERE backend=:backend AND parentTodo=:parentTodo;";
    args.insert( "backend", m_backend );
    args.insert( "parentTodo", m_currentTodo.id.toString() );
    return true;
}

void RecursiveDeleteTodoQuery::recordAvailable(const QVariantMap &record)
{
    TodoStruct todo = todoFromRecord( record );
    m_todosToDelete.enqueue( todo );
}

bool RecursiveDeleteTodoQuery::hasNext() const
{
    return !m_todosToDelete.isEmpty();
}

void RecursiveDeleteTodoQuery::endRun()
{
    worker()->deleteTodo( m_backend, m_currentTodo );
}


RecursiveDeleteTodoListQuery::RecursiveDeleteTodoListQuery(const QString &backend,
                                                           const TodoListStruct &list,
                                                           QObject *parent) :
    TodoListStorageQuery( parent ),
    m_backend( backend ),
    m_todoList( list ),
    m_todosToDelete()
{
}

void RecursiveDeleteTodoListQuery::beginRun()
{
    emit notifyTodoListDeleted( m_backend, m_todoList );
}

bool RecursiveDeleteTodoListQuery::query(QString &query, QVariantMap &args)
{
    query = "SELECT * FROM todo WHERE backend=:backend AND todoList=:todoList;";
    args.insert( "backend", m_backend );
    args.insert( "todoList", m_todoList.id.toString() );
    return true;
}

void RecursiveDeleteTodoListQuery::recordAvailable(const QVariantMap &record)
{
    TodoStruct todo = todoFromRecord( record );
    emit notifyTodoDeleted( m_backend, todo );
    m_todosToDelete.enqueue( todo );
}

void RecursiveDeleteTodoListQuery::endRun()
{
    worker()->deleteTodoList( m_backend, m_todoList );
    while ( !m_todosToDelete.isEmpty() ) {
        TodoStruct todo = m_todosToDelete.dequeue();
        worker()->deleteTodo( m_backend, todo );
    }
}

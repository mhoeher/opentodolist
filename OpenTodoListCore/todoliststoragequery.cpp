#include "todoliststoragequery.h"

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

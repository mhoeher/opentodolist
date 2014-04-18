#include "todoliststorage.h"

#include "todoliststoragequery.h"

#include <QDebug>
#include <QJsonDocument>
#include <QSqlError>
#include <QSqlQuery>
#include <QStringList>
#include <QVariantMap>

/**
   @brief Constructor
 */
TodoListStorage::TodoListStorage(QObject *parent) :
    QObject(parent),
    m_thread(),
    m_worker( new TodoListStorageWorker() ),
    m_queries(),
    m_queriesLock()
{
    m_thread.start();
    m_worker->moveToThread( &m_thread );
    QMetaObject::invokeMethod( m_worker, "init", Qt::QueuedConnection );
}

/**
   @brief Destructor
 */
TodoListStorage::~TodoListStorage()
{
    m_thread.exit();
    m_thread.wait();
    delete m_worker;
}

/**
   @brief Schedules the @p query for execution

   This schedules the given query for execution in the data base background
   thread. The TodoListStorage object takes over ownership of the query.
   Hence, upon running it, the query will automatically be deleted.
 */
void TodoListStorage::runQuery(TodoListStorageQuery *query)
{
    QMutexLocker l( &m_queriesLock );
    query->moveToThread( &m_thread );
    m_queries.enqueue( query );
}

/**
   @brief Inserts a todo list
   @param backend The backend the list belongs to
   @param list The todo list to insert
   @return True if a query to insert the todo has been scheduled successfully
 */
bool TodoListStorage::insertTodoList(const QString &backend, const TodoListStruct &list)
{
    return QMetaObject::invokeMethod(
                m_worker,
                "insertTodoList",
                Qt::QueuedConnection,
                Q_ARG( QString, backend ),
                Q_ARG( TodoListStruct, list ) );
}

/**
   @brief Inserts a todo
   @param backend The backend the todo belongs to
   @param todo The todo to insert
   @return True of a query to insert the todo has been scheduled successfully
 */
bool TodoListStorage::insertTodo(const QString &backend, const TodoStruct &todo)
{
    return QMetaObject::invokeMethod(
                m_worker,
                "insertTodo",
                Qt::QueuedConnection,
                Q_ARG( QString, backend ),
                Q_ARG( TodoStruct, todo ) );
}

/**
   @brief Removes a todo list
   @param backend The backend the list belongs to
   @param list The list to remove
   @return True of a query to remove the list has been scheduled successfully
 */
bool TodoListStorage::deleteTodoList(const QString &backend, const TodoListStruct &list)
{
    return QMetaObject::invokeMethod(
                m_worker,
                "deleteTodoList",
                Qt::QueuedConnection,
                Q_ARG( QString, backend ),
                Q_ARG( TodoListStruct, list ) );
}

/**
   @brief Removes a todo
   @param backend The backend the todo belongs to
   @param todo The todo to remove
   @return True if a query to remove the todo has been scheduled successfully
 */
bool TodoListStorage::deleteTodo(const QString &backend, const TodoStruct &todo)
{
    return QMetaObject::invokeMethod(
                m_worker,
                "deleteTodo",
                Qt::QueuedConnection,
                Q_ARG( QString, backend ),
                Q_ARG( TodoStruct, todo ) );
}


/**
   @brief Constructor
 */
TodoListStorageWorker::TodoListStorageWorker() :
    QObject(),
    m_dataBase(),
    m_dataBaseFile(),
    m_initialized( false )
{
}

/**
   @brief Destructor
 */
TodoListStorageWorker::~TodoListStorageWorker() {
    m_dataBase.close();
    qDebug() << "Closed database";
    m_dataBaseFile.close();
    qDebug() << "Deleted database";
}

/**
   @brief Initialize the worker's resources
 */
void TodoListStorageWorker::init() {
    if ( m_initialized ) {
        return;
    }

    m_dataBase = QSqlDatabase::addDatabase( "QSQLITE" );
    m_dataBaseFile.open();

    m_dataBaseFile.open();
    m_dataBase.setDatabaseName( m_dataBaseFile.fileName() );
    if ( !m_dataBase.open() ) {
        qCritical() << "Failed to open SQlite Database:"
                    << m_dataBaseFile.fileName();
    } else {
        qDebug() << "Opened temporary SQlite database"
                 << m_dataBaseFile.fileName();
        QSqlQuery createTodoListQuery( "CREATE TABLE todoList ("
                                       " backend VARCHAR NOT NULL,"
                                       " id VARCHAR NOT NULL,"
                                       " name TEXT NOT NULL,"
                                       " meta LONGTEXT );",
                                       m_dataBase );
        if ( !createTodoListQuery.lastError().isValid() ) {
            qDebug() << "Successfully created todoList table";
        } else {
            qCritical() << "Failed to create todoList table"
                        << createTodoListQuery.lastError().text();
        }

        QSqlQuery createTodoQuery( "CREATE TABLE todo ("
                                   " backend VARCHAR NOT NULL,"
                                   " id VARCHAR NOT NULL,"
                                   " weight DOUBLE NOT NULL,"
                                   " progress INT NOT NULL,"
                                   " priority INT,"
                                   " parentTodo VARCHAR,"
                                   " todoList VARCHAR NOT NULL,"
                                   " dueDate DATETIME,"
                                   " title TEXT,"
                                   " description LONGTEXT,"
                                   " deleted UNSIGNED BIT NOT NULL,"
                                   " meta LONGTEXT );",
                                   m_dataBase );
        if ( !createTodoQuery.lastError().isValid() ) {
            qDebug() << "Successfully created todo table";
        } else {
            qCritical() << "Failed to create todo table"
                        << createTodoQuery.lastError().text();
        }
    }

    m_initialized = true;
}

/**
   @brief Inserts a todo @p list of the @p backend into the database
 */
void TodoListStorageWorker::insertTodoList(const QString &backend, const TodoListStruct &list) {
    // ensure previous instance - if it exists - is removed
    deleteTodoList( backend, list );

    QVariantMap values;
    values.insert( "backend", backend );
    values.insert( "id", list.id.toString() );
    values.insert( "name", list.name );
    if ( !list.meta.isEmpty() ) {
        values.insert( "meta",
                       QJsonDocument::fromVariant( list.meta ).toJson() );
    }
    insertRow( "todoList", values );
}

/**
   @brief Inserts a @p todo of the @p backend into the database
 */
void TodoListStorageWorker::insertTodo(const QString &backend, const TodoStruct &todo) {
    // ensure previous instance - if it exists - is removed
    deleteTodo( backend, todo );

    QVariantMap values;
    values.insert( "backend", backend );
    values.insert( "id", todo.id.toString() );
    values.insert( "weight", todo.weight );
    values.insert( "progress", todo.progress );
    if ( todo.priority >= 0 && todo.priority <= 10 ) {
        values.insert( "priority", todo.priority );
    }
    if ( !todo.parentTodoId.isNull() ) {
        values.insert( "parentTodo", todo.parentTodoId.toString() );
    }
    values.insert( "todoList", todo.todoListId.toString() );
    if ( !todo.dueDate.isNull() ) {
        values.insert( "dueDate", todo.dueDate );
    }
    values.insert( "title", todo.title );
    values.insert( "description", todo.description );
    values.insert( "deleted", todo.deleted );
    if ( !todo.meta.isEmpty() ) {
        values.insert( "meta",
                       QJsonDocument::fromVariant( todo.meta )
                       .toJson() );
    }
    insertRow( "todo", values );
}

/**
   @brief Removes a todo @p list of the @p backend from the database
 */
void TodoListStorageWorker::deleteTodoList(const QString &backend, const TodoListStruct &list) {
    QVariantMap row;
    row.insert( "backend", backend );
    row.insert( "id", list.id );
    deleteRow( "todoList", row );
}

/**
   @brief Removes a @p todo of the @p backend from the database
 */
void TodoListStorageWorker::deleteTodo(const QString &backend, const TodoStruct &todo) {
    QVariantMap row;
    row.insert( "backend", backend );
    row.insert( "id", todo.id );
    deleteRow( "todo", row );
}

/**
   @brief Inserts a row in the database
   @param tableName The table to insert into
   @param row A key-value map describing the row values to insert
 */
void TodoListStorageWorker::insertRow(const QString &tableName, const QVariantMap &row) {
    QStringList columns;
    QStringList values;
    foreach ( const QString& key, row.keys() ) {
        columns << key;
        values << ":" + key;
    }
    QSqlQuery query( m_dataBase );
    query.prepare( QString( "INSERT INTO %1 (%2) VALUES (%3);" )
                   .arg( tableName )
                   .arg( columns.join( ", " ) )
                   .arg( values.join( ", " ) ) );
    foreach ( const QString &column, columns ) {
        query.bindValue( ":" + column, row.value( column ) );
    }
    if ( !query.exec() ) {
        qCritical() << "Failed to insert row:"
                    << query.lastError().text();
    }
}

/**
   @brief Removes a row from the table
   @param tableName The table to remove from
   @param row A key-value map describing the records (attribute=value) to remove
 */
void TodoListStorageWorker::deleteRow(const QString &tableName, const QVariantMap &row) {
    QStringList values;
    foreach ( QString key, row.keys() ) {
        values << key + "=:" + key;
    }
    QSqlQuery query( m_dataBase );
    query.prepare( QString( "DELETE FROM %1 WHERE %2;" )
                   .arg( tableName )
                   .arg( values.join( " AND " ) ) );
    foreach ( const QString &key, row.keys() ) {
        query.bindValue( key, row.value( key ) );
    }
    if ( !query.exec() ) {
        qCritical() << "Failed to remove row:"
                    << query.lastError().text();
    }
}

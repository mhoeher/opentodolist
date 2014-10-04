#include "database/database.h"

#include "database/storagequery.h"
#include "database/databaseworker.h"

#include <QDebug>
#include <QJsonDocument>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QStringList>
#include <QStandardPaths>
#include <QVariantMap>

namespace OpenTodoList {

namespace DataBase {

/**
   @brief Constructor
 */
Database::Database(QObject *parent) :
    QObject(parent),
    m_workerThread(),
    m_worker( new DatabaseWorker() ),
    m_backendPlugins( new PluginsLoader<IBackend>( "opentodobackends", this ) ),
    m_backendsThread(),
    m_backends()
{
    qDebug() << "Starting Database Worker thread";
    m_workerThread.start();

    qDebug() << "Initialiying database";
    m_worker->moveToThread( &m_workerThread );
    connect( m_worker, SIGNAL(todoListInserted(QString,TodoListStruct)),
             this, SIGNAL(todoListInserted(QString,TodoListStruct)) );
    connect( m_worker, SIGNAL(todoInserted(QString,TodoStruct)),
             this, SIGNAL(todoInserted(QString,TodoStruct)) );
    connect( m_worker, SIGNAL(todoListRemoved(QString,TodoListStruct)),
             this, SIGNAL(todoListRemoved(QString,TodoListStruct)) );
    connect( m_worker, SIGNAL(todoRemoved(QString,TodoStruct)),
             this, SIGNAL(todoRemoved(QString,TodoStruct)) );
    QMetaObject::invokeMethod( m_worker, "init", Qt::QueuedConnection );

    qDebug() << "Initialiying backends...";
    m_backends.reserve( m_backendPlugins->plugins().size() );
    m_backendsThread.start();
    for ( IBackend *interface : m_backendPlugins->plugins() ) {
        qDebug() << "Initializing backend" << interface->name();
        BackendWrapper *wrapper = new BackendWrapper( this, interface );
        wrapper->setLocalStorageDirectory( localStorageLocation( wrapper->id() ) );
        wrapper->moveToThread( &m_backendsThread );
        connect( this, SIGNAL(startBackends()), wrapper, SLOT(doStart()) );
        connect( this, SIGNAL(stopBackends()), wrapper, SLOT(doStop()), Qt::BlockingQueuedConnection );
        m_backends << wrapper;
    }

    qDebug() << "Starting backends";
    start();
}

/**
   @brief Destructor
 */
Database::~Database()
{
    qDebug() << "Stopping Backends";
    stop();

    qDebug() << "Stopping Backends Thread";
    m_backendsThread.quit();
    m_backendsThread.wait();

    qDebug() << "Deleting Backends";
    foreach ( BackendWrapper* wrapper, m_backends ) {
        delete wrapper;
    }

    qDebug() << "Stopping Database Thread";
    m_workerThread.quit();
    m_workerThread.wait();

    qDebug() << "Deleting Database";
    delete m_worker;
}

/**
   @brief Schedules the @p query for execution

   This schedules the given query for execution in the data base background
   thread. The Database object takes over ownership of the query.
   Hence, upon running it, the query will automatically be deleted.
 */
void Database::runQuery(StorageQuery *query)
{
    qDebug() << "Scheduling query" << query << "for execution";
    query->moveToThread( &m_workerThread );
    m_worker->run( query );
}

/**
   @brief Inserts a todo list
   @param backend The backend the list belongs to
   @param list The todo list to insert
   @return True if a query to insert the todo has been scheduled successfully
 */
bool Database::insertTodoList(const QString &backend, const ITodoList *list)
{
    return QMetaObject::invokeMethod(
                m_worker,
                "insertTodoList",
                Qt::QueuedConnection,
                Q_ARG( QString, backend ),
                Q_ARG( const ITodoList*, list ) );
}

/**
   @brief Inserts a todo
   @param backend The backend the todo belongs to
   @param todo The todo to insert
   @return True of a query to insert the todo has been scheduled successfully
 */
bool Database::insertTodo(const QString &backend, const ITodo *todo)
{
    return QMetaObject::invokeMethod(
                m_worker,
                "insertTodo",
                Qt::QueuedConnection,
                Q_ARG( QString, backend ),
                Q_ARG( const ITodo*, todo ) );
}

/**
   @brief Removes a todo list
   @param backend The backend the list belongs to
   @param list The list to remove
   @return True of a query to remove the list has been scheduled successfully
 */
bool Database::deleteTodoList(const QString &backend, const ITodoList *list)
{
    return QMetaObject::invokeMethod(
                m_worker,
                "deleteTodoList",
                Qt::QueuedConnection,
                Q_ARG( QString, backend ),
                Q_ARG( const ITodoList*, list ) );
}

/**
   @brief Removes a todo
   @param backend The backend the todo belongs to
   @param todo The todo to remove
   @return True if a query to remove the todo has been scheduled successfully
 */
bool Database::deleteTodo(const QString &backend, const ITodo *todo)
{
    return QMetaObject::invokeMethod(
                m_worker,
                "deleteTodo",
                Qt::QueuedConnection,
                Q_ARG( QString, backend ),
                Q_ARG( const ITodo*, todo ) );
}

void Database::start()
{
    emit startBackends();
}

void Database::stop()
{
    emit stopBackends();
}

#ifdef Q_OS_ANDROID
/**
   @brief Returns the external data location on Android

   This returns a directory where data can be put on Android that shall
   remain intact even when the application is uninstalled and installed later
   on.

   @todo Hardcoded package name - this is not portable.

   @return The external storage location on Android
 */
QString Database::androidExtStorageLocation()
{
    QString s( qgetenv( "EXTERNAL_STORAGE" ) );
    QDir dir( s + "/data/net.rpdev.opentodolist/" );
    return dir.absolutePath();
}
#endif

/**
   @brief Returns the directory where a given backend should write to

   This returns a directory that shall be used by the backend of the given
   @p type to write data into.

   @return A location where todo lists for the given @p type can be stored or an empty string
           if no fitting location could be found.
 */
QString Database::localStorageLocation(const QString &type)
{
    QStringList locations = QStandardPaths::standardLocations( QStandardPaths::DataLocation );

#ifdef Q_OS_ANDROID
    locations.insert( 0, androidExtStorageLocation() );
#endif

    foreach ( QString location, locations ) {
        QDir dir( location + "/" + type );
        if ( !dir.exists() ) {
            dir.mkpath( dir.absolutePath() );
        }
        if ( dir.exists() ) {
            return dir.path();
        }
    }
    return QString();
}

} /* DataBase */

} /* OpenTodoList */

#include "database/database.h"
#include "database/storagequery.h"
#include "database/databaseworker.h"

#include "database/queries/insertbackend.h"

#include "datamodel/backend.h"

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
    connect( m_worker, &DatabaseWorker::initialized, this, &Database::startBackends, Qt::QueuedConnection );
    QMetaObject::invokeMethod( m_worker, "init", Qt::QueuedConnection );

    // setup event broadcasting
    connect( m_worker, &DatabaseWorker::backendChanged, this, &Database::backendChanged );
    connect( m_worker, &DatabaseWorker::accountChanged, this, &Database::accountChanged );
    connect( m_worker, &DatabaseWorker::todoListChanged, this, &Database::todoListChanged );
    connect( m_worker, &DatabaseWorker::todoChanged, this, &Database::todoChanged );
    connect( m_worker, &DatabaseWorker::taskChanged, this, &Database::taskChanged );
    connect( m_worker, &DatabaseWorker::accountDeleted, this, &Database::accountDeleted );
    connect( m_worker, &DatabaseWorker::todoListDeleted, this, &Database::todoListDeleted );
    connect( m_worker, &DatabaseWorker::todoDeleted, this, &Database::todoDeleted );
    connect( m_worker, &DatabaseWorker::taskDeleted, this, &Database::taskDeleted );

    qDebug() << "Initializing backends...";
    m_backends.reserve( m_backendPlugins->plugins().size() );
    m_backendsThread.start();
    for ( IBackend *interface : m_backendPlugins->plugins() ) {
        qDebug() << "Initializing backend" << interface->title();
        BackendWrapper *wrapper = new BackendWrapper( this, interface );
        wrapper->setLocalStorageDirectory( localStorageLocation( wrapper->name() ) );
        wrapper->moveToThread( &m_backendsThread );
        m_backends << wrapper;
    }
}

/**
   @brief Destructor
 */
Database::~Database()
{
    qDebug() << "Stopping Backends";
    for ( BackendWrapper* wrapper : m_backends ) {
        qDebug() << "Stopping backend" << wrapper->name();
        if ( !QMetaObject::invokeMethod( wrapper, "doStop", Qt::BlockingQueuedConnection ) ) {
            qWarning() << "Failed to stop backend" << wrapper->name();
        }
    }

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
   @brief Runs a @p query

   This will run the query in the current thread. Note that this might block the current thread
   (hence avoid usage in the front end). Parentship of the query remains untouched - the Database
   object will not free the query after running the query finished.
 */
void Database::runQuery(StorageQuery *query)
{
    qDebug() << "Running query" << query;
    Q_ASSERT( query != nullptr );
    m_worker->run( query );
}

/**
   @brief Schedules the @p query for execution

   This schedules the given query for execution in the data base background
   thread. The Database object takes over ownership of the query.
   Hence, upon running it, the query will automatically be deleted.
 */
void Database::scheduleQuery(StorageQuery *query)
{
    qDebug() << "Scheduling query" << query << "for execution";
    Q_ASSERT( query != nullptr );
    query->moveToThread( &m_workerThread );
    m_worker->schedule( query );
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

void Database::startBackends()
{
    qDebug() << "Starting backends";
    for ( BackendWrapper* wrapper : m_backends ) {
        qDebug() << "Inserting/updating backend data in DB";
        DataModel::Backend *backend = new DataModel::Backend();
        backend->setName( wrapper->name() );
        backend->setTitle( wrapper->title() );
        backend->setDescription( wrapper->description() );
        Queries::InsertBackend *query = new Queries::InsertBackend( backend );
        m_worker->run( query );
        delete query;

        qDebug() << "Starting backend" << wrapper->name();
        if ( !QMetaObject::invokeMethod( wrapper, "doStart", Qt::QueuedConnection ) ) {
            qWarning() << "Failed to start backend" << wrapper->name();
        }
    }
}

} /* DataBase */

} /* OpenTodoList */

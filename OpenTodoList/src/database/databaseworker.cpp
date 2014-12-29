#include "database/databaseworker.h"
#include "database/storagequery.h"

#include <QDebug>
#include <QJsonDocument>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlQuery>

namespace OpenTodoList {

namespace DataBase {

/**
   @brief Constructor
 */
DatabaseWorker::DatabaseWorker() :
    QObject(),
    m_dataBase(),
    m_dataBaseFile(),
    m_initialized( false ),
    m_queue(),
    m_queueLock(),
    m_runLock()
{
}

/**
   @brief Destructor
 */
DatabaseWorker::~DatabaseWorker() {
    m_dataBase.close();
    qDebug() << "Closed database";
    m_dataBaseFile.close();
    qDebug() << "Deleted database";
}

/**
   @brief Runs a query

   This will run the @p query. Execution happens in the calling thread. The query will not be
   deleted after execution (in contrary to using DatabaseWorker::schedule()).
 */
void DatabaseWorker::run(StorageQuery *query)
{
    runQuery( query );
}

/**
   @brief Schedules a query

   Calling this method will enqueue the @p query and execute it later in the
   Database worker's thread.
 */
void DatabaseWorker::schedule(StorageQuery *query)
{
    QMutexLocker l( &m_queueLock );
    if ( query ) {
        m_queue.enqueue( query );
        QMetaObject::invokeMethod( this, "next", Qt::QueuedConnection );
    }
}

/**
   @brief Initialize the worker's resources
 */
void DatabaseWorker::init() {
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

        // Table for storing application data
        runSimpleQuery( "CREATE TABLE applicationInfo ( "
                        " key VARCHAR NOT NULL,"
                        " value VARCHAR NOT NULL )",
                        "Failed to create table applicationInfo" );

        // Tables for storing backend information:
        runSimpleQuery( "CREATE TABLE backend ("
                        " id INTEGER NOT NULL,"
                        " name VARCHAR NOT NULL,"
                        " title VARCHAR,"
                        " description VARCHAR,"
                        " PRIMARY KEY ( id ),"
                        " UNIQUE ( name )"
                        ");",
                        "Failed to create table backend" );

        // Tables for storing account information:
        runSimpleQuery( "CREATE TABLE account ("
                        " id INTEGER NOT NULL,"
                        " uuid VARCHAR NOT NULL,"
                        " name VARCHAR,"
                        " backend INTEGER NOT NULL,"
                        " dirty INTEGER,"
                        " disposed BOOLEAN,"
                        " PRIMARY KEY (id),"
                        " FOREIGN KEY (backend) REFERENCES backend ( id ),"
                        " UNIQUE ( uuid )"
                        ");",
                        "Failed to create table account" );
        runSimpleQuery( "CREATE TABLE accountMetaAttributeName ("
                        " id INTEGER NOT NULL,"
                        " name VARCHAR NOT NULL,"
                        " PRIMARY KEY ( id ),"
                        " UNIQUE ( name )"
                        ");",
                        "Failed to create table accountMetaAttributeName" );
        runSimpleQuery( "CREATE TABLE accountMetaAttribute ("
                        " account INTEGER NOT NULL,"
                        " attributeName INTEGER NOT NULL,"
                        " value VARCHAR,"
                        " PRIMARY KEY ( account, attributeName ),"
                        " FOREIGN KEY ( account ) REFERENCES account ( id ),"
                        " FOREIGN KEY ( attributeName ) REFERENCES accountMetaAttributeName ( id )"
                        ")",
                        "Failed to create table accountMetaAttribute" );

        // Tables for storing todo list information:
        runSimpleQuery( "CREATE TABLE todoList ("
                        " id INTEGER NOT NULL,"
                        " uuid VARCHAR NOT NULL,"
                        " account INTEGER NOT NULL,"
                        " name TEXT,"
                        " dirty INTEGER,"
                        " disposed BOOLEAN,"
                        " PRIMARY KEY ( id ),"
                        " UNIQUE ( uuid ),"
                        " FOREIGN KEY ( account ) REFERENCES account ( id )"
                        ");",
                        "Failed to create table todoList" );
        runSimpleQuery( "CREATE TABLE todoListMetaAttributeName ("
                        " id INTEGER NOT NULL,"
                        " name VARCHAR NOT NULL,"
                        " PRIMARY KEY ( id ),"
                        " UNIQUE ( name )"
                        ");",
                        "Failed to create table todoListMetaAttributeName" );
        runSimpleQuery( "CREATE TABLE todoListMetaAttribute ("
                        " todoList INTEGER NOT NULL,"
                        " attributeName INTEGER NOT NULL,"
                        " value VARCHAR NOT NULL,"
                        " PRIMARY KEY ( todoList, attributeName ),"
                        " FOREIGN KEY ( todoList ) REFERENCES todoList ( id ),"
                        " FOREIGN KEY ( attributeName ) REFERENCES todoListMetaAttributeName ( id )"
                        ");",
                        "Failed to create table todoListMetaAttribute" );

        // Tables for storing todo information:
        runSimpleQuery( "CREATE TABLE todo ("
                        " id INTEGER NOT NULL,"
                        " uuid VARCHAR NOT NULL,"
                        " weight DOUBLE,"
                        " done BOOLEAN,"
                        " priority INT,"
                        " dueDate DATETIME,"
                        " title TEXT,"
                        " description LONGTEXT,"
                        " dirty INTEGER,"
                        " disposed BOOLEAN,"
                        " todoList INTEGER NOT NULL,"
                        " PRIMARY KEY ( id ),"
                        " UNIQUE ( uuid ),"
                        " FOREIGN KEY ( todoList ) REFERENCES todoList ( id )"
                        ");",
                        "Failed to create table todo" );
        runSimpleQuery( "CREATE TABLE todoMetaAttributeName ("
                        " id INTEGER NOT NULL,"
                        " name VARCHAR NOT NULL,"
                        " PRIMARY KEY ( id ),"
                        " UNIQUE ( name )"
                        ");",
                        "Failed to create table todoMetaAttributeName" );
        runSimpleQuery( "CREATE TABLE todoMetaAttribute ("
                        " todo INTEGER NOT NULL,"
                        " attributeName INTEGER NOT NULL,"
                        " value VARCHAR,"
                        " PRIMARY KEY ( todo, attributeName ),"
                        " FOREIGN KEY ( todo ) REFERENCES todo ( id ),"
                        " FOREIGN KEY ( attributeName ) REFERENCES todoMetaAttributeName ( id )"
                        ");",
                        "Failed to create table todoMetaAttribute" );

        // Tables for storing task information:
        runSimpleQuery( "CREATE TABLE task ("
                        " id INTEGER NOT NULL,"
                        " uuid VARCHAR NOT NULL,"
                        " weight DOUBLE,"
                        " done BOOLEAN,"
                        " title TEXT,"
                        " dirty INTEGER,"
                        " disposed BOOLEAN,"
                        " todo INTEGER NOT NULL,"
                        " PRIMARY KEY ( id ),"
                        " UNIQUE ( uuid ),"
                        " FOREIGN KEY ( todo ) REFERENCES todo ( id )"
                        ");",
                        "Failed to create table todo" );
        runSimpleQuery( "CREATE TABLE taskMetaAttributeName ("
                        " id INTEGER NOT NULL,"
                        " name VARCHAR NOT NULL,"
                        " PRIMARY KEY ( id ),"
                        " UNIQUE ( name )"
                        ");",
                        "Failed to create table taskMetaAttributeName" );
        runSimpleQuery( "CREATE TABLE taskMetaAttribute ("
                        " task INTEGER NOT NULL,"
                        " attributeName INTEGER NOT NULL,"
                        " value VARCHAR,"
                        " PRIMARY KEY ( task, attributeName ),"
                        " FOREIGN KEY ( task ) REFERENCES task ( id ),"
                        " FOREIGN KEY ( attributeName ) REFERENCES taskMetaAttributeName ( id )"
                        ");",
                        "Failed to create table taskMetaAttribute" );
    }

    m_initialized = true;

    emit initialized();
}

void DatabaseWorker::runSimpleQuery(const QString &query, const QString &errorMsg )
{
    QSqlQuery q( query, m_dataBase );
    if ( q.lastError().isValid() ) {
        qCritical() << errorMsg
                    << q.lastError().text();
    }
}

/**
   @brief Runs the @p query
 */
void DatabaseWorker::runQuery(StorageQuery *query)
{
    QMutexLocker l( &m_runLock );
    do {
        connect( query, &StorageQuery::backendChanged,
                 this, &DatabaseWorker::backendChanged, Qt::QueuedConnection );
        connect( query, &StorageQuery::accountChanged,
                 this, &DatabaseWorker::accountChanged, Qt::QueuedConnection );
        connect( query, &StorageQuery::todoListChanged,
                 this, &DatabaseWorker::todoListChanged, Qt::QueuedConnection );
        connect( query, &StorageQuery::todoChanged,
                 this, &DatabaseWorker::todoChanged, Qt::QueuedConnection );
        connect( query, &StorageQuery::taskChanged,
                 this, &DatabaseWorker::taskChanged, Qt::QueuedConnection );
        query->m_worker = this;
        query->beginRun();
        QString queryStr;
        QVariantMap values;
        bool validQuery = query->query( queryStr, values );
        if ( validQuery ) {
            QSqlQuery q( m_dataBase );
            q.prepare( queryStr );
            foreach ( QString key, values.keys() ) {
                q.bindValue( ":" + key, values.value( key ) );
            }
            if ( q.exec() ) {
                while ( q.next() ) {
                    QVariantMap recordData;
                    for ( int i = 0; i < q.record().count(); ++i ) {
                        recordData.insert( q.record().fieldName( i ),
                                           q.record().value( i ) );
                    }
                    query->recordAvailable( recordData );
                }
                if ( q.lastInsertId().isValid() ) {
                    query->newIdAvailable( q.lastInsertId() );
                }
            } else {
                qWarning() << q.lastError().text();
                qWarning() << q.executedQuery();
                qWarning() << queryStr;
                qWarning() << values;
            }
        }
        query->endRun();
    } while ( query->hasNext() );
}

/**
   @brief Executes the next query in the queue

   This will run the next query from the queue and delete it after the run.
 */
void DatabaseWorker::next()
{
    QMutexLocker l( &m_queueLock );
    StorageQuery *query = 0;
    if ( !m_queue.isEmpty() ) {
        query = m_queue.dequeue();
    }
    if ( query ) {
        runQuery( query );
        delete query;
    }
}

} /* DataBase */

} /* OpenTodoList */

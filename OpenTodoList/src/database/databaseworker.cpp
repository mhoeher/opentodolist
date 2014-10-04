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
    m_queueLock()
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
   @brief Actually runs a query
 */
void DatabaseWorker::run(StorageQuery *query)
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

        // Tables for storing backend information:
        runSimpleQuery( "CREATE TABLE backend ("
                        " id INTEGER NOT NULL,"
                        " name VARCHAR NOT NULL,"
                        " title VARCHAR NOT NULL,"
                        " description VARCHAR NOT NULL,"
                        " PRIMARY KEY ( id ),"
                        " UNIQUE ( name )"
                        ");",
                        "Failed to create table backend" );

        // Tables for storing account information:
        runSimpleQuery( "CREATE TABLE account ("
                        " id INTEGER NOT NULL,"
                        " uuid VARCHAR NOT NULL,"
                        " name VARCHAR NOT NULL,"
                        " backend INTEGER NOT NULL,"
                        " dirty BOOLEAN NOT NULL,"
                        " disposed BOOLEAN NOT NULL,"
                        " lastModificationTime DATETIME,"
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
                        " value VARCHAR NOT NULL,"
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
                        " name TEXT NOT NULL,"
                        " dirty BOOLEAN NOT NULL,"
                        " disposed BOOLEAN NOT NULL,"
                        " lastModificationDate DATETIME,"
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
                        " weight DOUBLE NOT NULL,"
                        " progress INT NOT NULL,"
                        " priority INT,"
                        " dueDate DATETIME,"
                        " title TEXT,"
                        " description LONGTEXT,"
                        " deleted UNSIGNED BIT NOT NULL,"
                        " dirty BOOLEAN NOT NULL,"
                        " disposed BOOLEAN NOT NULL,"
                        " todoList INTEGER NOT NULL,"
                        " parentTodo INTEGER,"
                        " lastModificationDate DATETIME,"
                        " PRIMARY KEY ( id ),"
                        " UNIQUE ( uuid ),"
                        " FOREIGN KEY ( todoList ) REFERENCES todoList ( id ),"
                        " FOREIGN KEY ( parentTodo ) REFERENCES todo ( id )"
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
                        " value VARCHAR NOT NULL,"
                        " PRIMARY KEY ( todo, attributeName ),"
                        " FOREIGN KEY ( todo ) REFERENCES todo ( id ),"
                        " FOREIGN KEY ( attributeName ) REFERENCES todoMetaAttributeName ( id )"
                        ");",
                        "Failed to create table todoMetaAttribute" );
    }

    m_initialized = true;
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
   @brief Executes the next query in the queue
 */
void DatabaseWorker::next()
{
    QMutexLocker l( &m_queueLock );
    StorageQuery *query = 0;
    if ( !m_queue.isEmpty() ) {
        query = m_queue.dequeue();
    }
    if ( query ) {
        do {
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
                } else {
                    qWarning() << q.lastError().text();
                    qWarning() << q.executedQuery();
                    qWarning() << queryStr;
                    qWarning() << values;
                }
            }
            query->endRun();
        } while ( query->hasNext() );
        delete query;
    }
}

} /* DataBase */

} /* OpenTodoList */

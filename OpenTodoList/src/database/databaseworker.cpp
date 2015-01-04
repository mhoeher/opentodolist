/*
 *  OpenTodoList - A todo and task manager
 *  Copyright (C) 2014 - 2015 Martin Höher <martin@rpdev.net>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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
DatabaseWorker::DatabaseWorker(const QString &dbLocation) :
  QObject(),
  m_dataBase(),
  m_dataBaseFile( dbLocation ),
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
  qDebug() << "Closed database file";
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

  if ( !m_dataBaseFile.open( QIODevice::ReadWrite ) ) {
    qWarning() << "Failed to open" << m_dataBaseFile.fileName()
               << "because of:" << m_dataBaseFile.errorString();
  } else {
    m_dataBase.setDatabaseName( m_dataBaseFile.fileName() );
    if ( !m_dataBase.open() ) {
      qCritical() << "Failed to open SQlite Database:"
                  << m_dataBaseFile.fileName();
    } else {
      qDebug() << "Opened temporary SQlite database"
               << m_dataBaseFile.fileName();

      // Various settings
      // NOTE: Needs to be run on every connection to the database (not just once when
      //       creating the tables).
      runSimpleQuery( "PRAGMA foreign_keys=ON;", "Failed to enable foreign key support" );

      // Read schema version
      QSqlQuery readSchemaVersionQuery( m_dataBase );
      int version = -1;
      if ( readSchemaVersionQuery.exec( "SELECT version FROM schemaVersion LIMIT 1;" ) &&
           readSchemaVersionQuery.next() ) {
        version = readSchemaVersionQuery.record().value( "version" ).toInt();
        readSchemaVersionQuery.finish();
      }

      switch ( version ) {
      case -1:
        updateToSchemaVersion0();
        break;

      case 0:
        qDebug() << "DB uses schema version 0. Nothing to be done to upgrade.";
        break;

      default:
        qCritical() << "The used database appears to use schema version" << version
                    << "of the application. This version belongs to a future version of"
                    << "the application! Please update the application.";
        break;
      }
    }

    m_initialized = true;
  }

  emit initialized();
}

void DatabaseWorker::runSimpleQuery(const QString &query, const QString &errorMsg)
{
  QSqlQuery q( query, m_dataBase );
  if ( q.lastError().isValid() ) {
    if ( !errorMsg.isEmpty() ) {
      qCritical() << errorMsg;
    }
    qCritical() << q.lastError().text();
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
    connect( query, &StorageQuery::accountDeleted,
             this, &DatabaseWorker::accountDeleted, Qt::QueuedConnection );
    connect( query, &StorageQuery::todoListDeleted,
             this, &DatabaseWorker::todoListDeleted, Qt::QueuedConnection );
    connect( query, &StorageQuery::todoDeleted,
             this, &DatabaseWorker::todoDeleted, Qt::QueuedConnection );
    connect( query, &StorageQuery::taskDeleted,
             this, &DatabaseWorker::taskDeleted, Qt::QueuedConnection );
    query->m_worker = this;
    query->beginRun();
    QString queryStr;
    QVariantMap values;
    int options = 0;
    bool validQuery = query->query( queryStr, values, options );
    if ( validQuery ) {
      if ( options & StorageQuery::QueryIsUpdateQuery ) {
        runSimpleQuery( "PRAGMA foreign_keys=0;" );
      }
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
      if ( options & StorageQuery::QueryIsUpdateQuery ) {
        runSimpleQuery( "PRAGMA foreign_keys=1;" );
      }
    }
    query->endRun();
  } while ( query->hasNext() );
  emit query->queryFinished();
}

/**
   @brief Updates the database to schema version 0
 */
void DatabaseWorker::updateToSchemaVersion0()
{
  // Table for storing application data
  runSimpleQuery( "CREATE TABLE schemaVersion ( "
                  " version INTEGER NOT NULL );",
                  "Failed to create table schemaVersion" );

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
                  " FOREIGN KEY (backend) REFERENCES backend ( id ) ON DELETE CASCADE,"
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
                  " FOREIGN KEY ( account ) REFERENCES account ( id ) ON DELETE CASCADE,"
                  " FOREIGN KEY ( attributeName ) REFERENCES accountMetaAttributeName ( id ) ON DELETE CASCADE"
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
                  " FOREIGN KEY ( account ) REFERENCES account ( id )  ON DELETE CASCADE"
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
                  " FOREIGN KEY ( todoList ) REFERENCES todoList ( id ) ON DELETE CASCADE,"
                  " FOREIGN KEY ( attributeName ) REFERENCES todoListMetaAttributeName ( id ) ON DELETE CASCADE"
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
                  " FOREIGN KEY ( todoList ) REFERENCES todoList ( id ) ON DELETE CASCADE"
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
                  " FOREIGN KEY ( todo ) REFERENCES todo ( id ) ON DELETE CASCADE,"
                  " FOREIGN KEY ( attributeName ) REFERENCES todoMetaAttributeName ( id ) ON DELETE CASCADE"
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
                  " FOREIGN KEY ( todo ) REFERENCES todo ( id ) ON DELETE CASCADE"
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
                  " FOREIGN KEY ( task ) REFERENCES task ( id ) ON DELETE CASCADE,"
                  " FOREIGN KEY ( attributeName ) REFERENCES taskMetaAttributeName ( id ) ON DELETE CASCADE"
                  ");",
                  "Failed to create table taskMetaAttribute" );

  runSimpleQuery( "INSERT INTO schemaVersion  (version) VALUES ( 0 );",
                  "Failed to save current schema version" );
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

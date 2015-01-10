/*
 *  OpenTodoList - A todo and task manager
 *  Copyright (C) 2013 - 2015 Martin Höher <martin@rpdev.net>
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

#include "localxmlbackend.h"

#include <QCryptographicHash>
#include <QDebug>
#include <QDirIterator>
#include <QDomDocument>
#include <QStringList>
#include <QtPlugin>

const QString LocalXmlBackend::TodoListConfigFileName = "config.xml";
const QString LocalXmlBackend::TodoDirectoryName = "todos";

const QString LocalXmlBackend::TodoListMetaFileName = "LocalXmlBackend::TodoList::fileName";
const QString LocalXmlBackend::TodoListMetaHash = "LocalXmlBackend::TodoList::hash";
const QString LocalXmlBackend::TodoMetaFileName = "LocalXmlBackend::Todo::fileName";
const QString LocalXmlBackend::TodoMetaHash = "LocalXmlBackend::Todo::hash";
const QString LocalXmlBackend::TaskMetaFileName = "LocalXmlBackend::Task::fileName";
const QString LocalXmlBackend::TaskMetaHash = "LocalXmlBackend::Task::hash";

LocalXmlBackend::LocalXmlBackend(QObject *parent) :
  QObject( parent ),
  m_database( nullptr ),
  m_localStorageDirectory( QString() ),
  m_account( nullptr )
{
  qDebug() << "Creating LocalXmlBackend";
}

LocalXmlBackend::~LocalXmlBackend()
{
  qDebug() << "Deleting LocalXmlBackend";
  if ( m_account ) {
    delete m_account;
  }
}

void LocalXmlBackend::setDatabase(OpenTodoList::IDatabase *database)
{
  m_database = database;
}

void LocalXmlBackend::setLocalStorageDirectory(const QString &directory)
{
  m_localStorageDirectory = directory;
  qDebug() << "Set local storage directory of" << name() << "to" << directory;
}

QString LocalXmlBackend::name() const
{
  return "LocalXmlDirectory";
}

QString LocalXmlBackend::title() const
{
  return tr( "Todo List in local files (XML format)" );
}

QString LocalXmlBackend::description() const
{
  return tr( "Stores your todos locally in a directory using an "
             "XML file format." );
}

QSet<IBackend::Capabilities> LocalXmlBackend::capabilities() const
{
  QSet<Capabilities> result;
  result << CanCreateTodoList
         << CanCreateTodo
         << CanCreateTask
         << CanDisposeTodoList
         << CanDisposeTodo
         << CanDisposeTask;
  return result;
}

bool LocalXmlBackend::start()
{
  qsrand( QDateTime::currentDateTime().toMSecsSinceEpoch() );
  QList<OpenTodoList::IAccount*> accounts = m_database->getAccounts(
        OpenTodoList::IDatabase::QueryAny, 1 );
  if ( accounts.isEmpty() ) {
    m_account = m_database->createAccount();
    m_account->setUuid( QUuid::createUuid() );
  } else {
    m_account = accounts.first();
  }
  m_account->setName( tr( "Local Todo Lists" ) );
  m_database->insertAccount( m_account );

  QStringList todoLists = locateTodoLists();
  for ( const QString &todoListFile : todoLists ) {
    fixTodoList( todoListFile );
    ITodoList *todoList = m_database->createTodoList();
    QDomDocument doc = documentForFile( todoListFile );
    QByteArray todoListHash;
    if ( domToTodoList( doc, todoList ) && todoListNeedsUpdate( todoList, todoListFile, todoListHash ) ) {
      todoList->insertMetaAttribute( TodoListMetaFileName, todoListFile );
      todoList->insertMetaAttribute( TodoListMetaHash, todoListHash );
      todoList->setAccount( m_account->uuid() );
      m_database->insertTodoList( todoList );
    }

    QStringList todos = locateTodos( todoListFile );
    for ( const QString &todoFile : todos ) {
      fixTodo( todoFile );
      ITodo *todo = m_database->createTodo();
      doc = documentForFile( todoFile );
      QByteArray todoHash;
      if ( domToTodo( doc, todo ) && todoNeedsUpdate( todo, todoFile, todoHash )) {
        todo->insertMetaAttribute( TodoMetaFileName, todoFile );
        todo->insertMetaAttribute( TodoMetaHash, todoHash );
        todo->setTodoList( todoList->uuid() );
        m_database->insertTodo( todo );
      }

      QStringList tasks = locateTasks( todoFile );
      for ( const QString &taskFile : tasks ) {
        ITask *task = m_database->createTask();
        doc = documentForFile( taskFile );
        QByteArray taskHash;
        if ( domToTask( doc, task ) && taskNeedsUpdate( task, taskFile, taskHash ) ) {
          task->insertMetaAttribute( TaskMetaFileName, taskFile );
          task->insertMetaAttribute( TaskMetaHash, taskHash );
          task->setTodo( todo->uuid() );
          m_database->insertTask( task );
        }
        delete task;
      }
      delete todo;
    }
    delete todoList;
  }
  return true;
}

bool LocalXmlBackend::stop()
{
  return true;
}

void LocalXmlBackend::sync()
{
  // Step 1: Remove disposed objects starting from top
  deleteTodoLists();
  deleteTodos();
  deleteTasks();
  // Step 2: Save modified objects
  saveTodoLists();
  saveTodos();
  saveTasks();
}

/**
   @brief Automatically locate todo lists

   Searches for all todo lists and returns them as a string list. Only lists
   relative to the local storage directory are returned.
 */
QStringList LocalXmlBackend::locateTodoLists() const
{
  QStringList result;
  QDir dir( m_localStorageDirectory );
  for ( QString entry : dir.entryList( QDir::Dirs | QDir::NoDotAndDotDot ) ) {
    QFileInfo fi( m_localStorageDirectory + "/" + entry + "/" + TodoListConfigFileName );
    if ( fi.isFile() ) {
      result << entry + "/" + TodoListConfigFileName;
    }
  }
  return result;
}

QStringList LocalXmlBackend::locateTodos(const QString &todoList) const
{
  QDir localStorageDir( m_localStorageDirectory );
  QFileInfo fi( m_localStorageDirectory + "/" + todoList );
  QDir dir = fi.dir();
  QStringList result;
  if ( dir.cd( TodoDirectoryName ) ) {
    QStringList entries = dir.entryList( { "*.xml" }, QDir::Files );
    for ( const QString &entry : entries ) {
      result << localStorageDir.relativeFilePath( dir.absoluteFilePath( entry ) );
    }
  }
  return result;
}

QStringList LocalXmlBackend::locateTasks(const QString &todo) const
{
  QStringList result;
  QDir localStorageDir( m_localStorageDirectory );
  QFileInfo fi( m_localStorageDirectory + "/" + todo );
  QDir dir = fi.dir();
  if ( dir.cd( fi.baseName() ) ) {
    QStringList entries = dir.entryList( { "*.xml" }, QDir::Files );
    for ( const QString &entry : entries ) {
      result << localStorageDir.relativeFilePath( dir.absoluteFilePath( entry ) );
    }
  }
  return result;
}

bool LocalXmlBackend::todoListToDom(const OpenTodoList::ITodoList *list, QDomDocument &doc)
{
  QDomElement root = doc.documentElement();
  if ( !root.isElement() ) {
    root = doc.createElement( "todoList" );
    doc.appendChild( root );
  }
  root.setAttribute( "id", list->uuid().toString() );
  root.setAttribute( "name", list->name() );
  return true;
}

bool LocalXmlBackend::domToTodoList(const QDomDocument &doc, OpenTodoList::ITodoList *list)
{
  QDomElement root = doc.documentElement();
  if ( !root.isElement() ) {
    return false;
  }
  list->setUuid( QUuid( root.attribute( "id", list->uuid().toString() ) ) );
  list->setName( root.attribute( "name", list->name() ) );
  return true;
}

bool LocalXmlBackend::todoToDom(const OpenTodoList::ITodo *todo, QDomDocument &doc)
{
  QDomElement root = doc.documentElement();
  if ( !root.isElement() ) {
    root = doc.createElement( "todo" );
    doc.appendChild( root );
  }
  root.setAttribute( "id", todo->uuid().toString() );
  root.setAttribute( "title", todo->title() );
  root.setAttribute( "done", todo->done() ? "true" : "false" );
  root.setAttribute( "priority", todo->priority() );
  root.setAttribute( "weight", todo->weight() );
  if ( todo->dueDate().isValid() ) {
    root.setAttribute( "dueDate", todo->dueDate().toString() );
  } else {
    root.removeAttribute( "dueDate" );
  }
  QDomElement descriptionElement = root.firstChildElement( "description" );
  if ( !descriptionElement.isElement() ) {
    descriptionElement = doc.createElement( "description" );
    root.appendChild( descriptionElement );
  }
  while ( !descriptionElement.firstChild().isNull() ) {
    descriptionElement.removeChild( descriptionElement.firstChild() );
  }
  QDomText descriptionText = doc.createTextNode( todo->description() );
  descriptionElement.appendChild( descriptionText );
  return true;
}

bool LocalXmlBackend::domToTodo(const QDomDocument &doc, OpenTodoList::ITodo *todo)
{
  QDomElement root = doc.documentElement();

  if ( !root.isElement() ) {
    return false;
  }

  todo->setUuid( QUuid( root.attribute( "id" ) ) );
  todo->setTitle( root.attribute( "title" ) );
  if ( root.hasAttribute( "done" ) ) {
    todo->setDone( root.attribute( "done", "true" ) == "true" );
  } else {
    // TODO: Remove this in 0.3 release
    todo->setDone( root.attribute( "progress", 0 ).toInt() >= 100 );
  }

  todo->setPriority( qBound( -1, root.attribute( "priority", QString::number(todo->priority()) ).toInt(), 10 ) );
  if ( root.hasAttribute( "dueDate" ) ) {
    todo->setDueDate( QDateTime::fromString( root.attribute( "dueDate" ) ) );
  } else {
    todo->setDueDate( QDateTime() );
  }
  todo->setWeight( root.attribute( "weight", QString::number( todo->weight() ) ).toDouble() );

  QDomElement description = root.firstChildElement( "description" );
  if ( description.isElement() ) {
    todo->setDescription( description.text() );
  }
  return true;
}

bool LocalXmlBackend::taskToDom(const ITask *task, QDomDocument &doc)
{
  QDomElement root = doc.documentElement();

  if ( !root.isElement() ) {
    root = doc.createElement( "task" );
    doc.appendChild( root );
  }

  root.setAttribute( "id", task->uuid().toString() );
  root.setAttribute( "done", task->done() ? "true" : "false" );
  root.setAttribute( "title", task->title() );
  root.setAttribute( "weight", task->weight() );

  return true;
}

bool LocalXmlBackend::domToTask(const QDomDocument &doc, ITask *task)
{
  QDomElement root = doc.documentElement();

  if ( !root.isElement() ) {
    return false;
  }

  task->setUuid( QUuid( root.attribute( "id" ) ));
  task->setDone( root.attribute( "done" ) == "true" );
  task->setTitle( root.attribute( "title" ) );
  task->setWeight( root.attribute( "weight" ).toDouble() );

  return true;
}

void LocalXmlBackend::deleteTodoLists()
{
  QList<ITodoList*> todoLists;
  do {
    todoLists = m_database->getTodoLists( IDatabase::QueryDisposed, 100 );
    for ( ITodoList *todoList : todoLists ) {
      QString fileName = m_localStorageDirectory + "/" +
          todoList->metaAttributes().value( TodoListMetaFileName, QString() ).toString();
      QFileInfo fi( fileName );
      if ( fi.exists() ) {
        QDir dir = fi.absoluteDir();
        dir.removeRecursively();
      }
      m_database->deleteTodoList( todoList );
      delete todoList;
    }
  } while ( !todoLists.isEmpty() );
}

void LocalXmlBackend::deleteTodos()
{
  QList<ITodo*> todos;
  do {
    todos = m_database->getTodos( IDatabase::QueryDisposed, 100 );
    for ( ITodo *todo : todos ) {
      QString fileName = m_localStorageDirectory + "/" +
          todo->metaAttributes().value( TodoMetaFileName, QString() ).toString();
      QFileInfo fi( fileName );
      if ( fi.exists() ) {
        QFile file( fileName );
        file.remove();
      }
      QFileInfo dfi( fi.absolutePath() + fi.baseName() );
      if ( dfi.exists() && dfi.isDir() ) {
        QDir dir( dfi.absoluteFilePath() );
        dir.removeRecursively();
      }
      m_database->deleteTodo( todo );
      delete todo;
    }
  } while ( !todos.isEmpty() );
}

void LocalXmlBackend::deleteTasks()
{
  QList<ITask*> tasks;
  do {
    tasks = m_database->getTasks( IDatabase::QueryDisposed, 100 );
    for ( ITask* task : tasks ) {
      QString fileName = m_localStorageDirectory + "/" +
          task->metaAttributes().value( TaskMetaFileName ).toString();
      QFileInfo fi( fileName );
      if ( fi.exists() && fi.isFile() ) {
        QFile file( fileName );
        file.remove();
      }
      m_database->deleteTask( task );
      delete task;
    }
  } while ( !tasks.isEmpty() );
}

void LocalXmlBackend::saveTodoLists()
{
  QList<ITodoList*> todoLists;
  do {
    todoLists = m_database->getTodoLists( IDatabase::QueryDirty, 100 );
    for ( ITodoList *todoList : todoLists ) {
      QString fileName = todoList->metaAttributes().value( TodoListMetaFileName ).toString();
      if ( fileName.isEmpty() ) {
        QDir localStorageDir( m_localStorageDirectory );
        if ( localStorageDir.mkdir( todoList->uuid().toString() ) &&
             localStorageDir.cd( todoList->uuid().toString() ) ) {
          QVariantMap attrs = todoList->metaAttributes();
          attrs.insert( TodoListMetaFileName,
                        QDir( m_localStorageDirectory ).relativeFilePath(
                          localStorageDir.absoluteFilePath( TodoListConfigFileName ) ) );
          todoList->setMetaAttributes( attrs );
        } else {
          qWarning() << "Unable to create local directory for todo list" << todoList->name();
        }
      }
      saveTodoList( todoList );
      m_database->onTodoListSaved(todoList);
      delete todoList;
    }
  } while ( !todoLists.isEmpty() );
}

void LocalXmlBackend::saveTodos()
{
  QList<ITodo*> todos;
  do {
    todos = m_database->getTodos( IDatabase::QueryDirty, 100 );
    for ( ITodo *todo : todos ) {
      QString fileName = todo->metaAttributes().value( TodoMetaFileName ).toString();
      if ( fileName.isEmpty() ) {
        ITodoList* todoList = m_database->getTodoList( todo->todoList() );
        if ( todoList ) {
          QString todoListFileName = m_localStorageDirectory + "/" +
              todoList->metaAttributes().value( TodoListMetaFileName ).toString();
          QFileInfo todoListFI( todoListFileName );
          if ( todoListFI.isFile() ) {
            QDir dir( todoListFI.absolutePath() );
            if ( dir.cd( TodoDirectoryName ) ||
                 ( dir.mkdir( TodoDirectoryName ) && dir.cd( TodoDirectoryName ) ) ) {
              QVariantMap attrs = todo->metaAttributes();
              attrs.insert( TodoMetaFileName, QDir( m_localStorageDirectory ).relativeFilePath(
                              dir.absoluteFilePath( todo->uuid().toString() + ".xml" ) ) );
              todo->setMetaAttributes( attrs );
            }
          }
          delete todoList;
        }
      }
      saveTodo( todo );
      m_database->onTodoSaved(todo);
      delete todo;
    }
  } while ( !todos.isEmpty() );
}

void LocalXmlBackend::saveTasks()
{
  QList<ITask*> tasks;
  do {
    tasks = m_database->getTasks( IDatabase::QueryDirty, 100 );
    for ( ITask *task : tasks ) {
      QString fileName = task->metaAttributes().value( TaskMetaFileName ).toString();
      if ( fileName.isEmpty() ) {
        ITodo* todo = m_database->getTodo( task->todo() );
        if ( todo ) {
          QString todoFileName = m_localStorageDirectory + "/" +
              todo->metaAttributes().value( TodoMetaFileName ).toString();
          QFileInfo todoFI( todoFileName );
          if ( todoFI.isFile() ) {
            QDir dir( todoFI.absolutePath() );
            if ( dir.cd( todo->uuid().toString() ) ||
                 ( dir.mkdir( todo->uuid().toString() ) && dir.cd( todo->uuid().toString() ) ) ) {
              QVariantMap attrs = task->metaAttributes();
              attrs.insert( TaskMetaFileName, QDir( m_localStorageDirectory ).relativeFilePath(
                              dir.absoluteFilePath( task->uuid().toString() + ".xml" ) ) );
              task->setMetaAttributes( attrs );
            }
          }
          delete todo;
        }
      }
      saveTask( task );
      m_database->onTaskSaved( task );
      delete task;
    }
  } while ( !tasks.isEmpty() );
}

void LocalXmlBackend::saveTodoList(ITodoList *todoList)
{
  QString fileName = todoList->metaAttributes().value( TodoListMetaFileName ).toString();
  if ( !fileName.isEmpty() ) {
    QDomDocument doc = documentForFile( fileName );
    if ( todoListToDom( todoList, doc ) ) {
      documentToFile( doc, fileName );
      todoList->insertMetaAttribute( TodoListMetaHash, hashForFile( fileName ) );
    }
  }
}

void LocalXmlBackend::saveTodo(ITodo *todo)
{
  QString fileName = todo->metaAttributes().value( TodoMetaFileName ).toString();
  if ( !fileName.isEmpty() ) {
    QDomDocument doc = documentForFile( fileName );
    if ( todoToDom( todo, doc ) ) {
      documentToFile( doc, fileName );
      todo->insertMetaAttribute( TodoMetaHash, hashForFile( fileName ) );
    }
  }
}

void LocalXmlBackend::saveTask(ITask *task)
{
  QString fileName = task->metaAttributes().value( TaskMetaFileName ).toString();
  if ( !fileName.isEmpty() ) {
    QDomDocument doc = documentForFile( fileName );
    if ( taskToDom( task, doc ) ) {
      documentToFile( doc, fileName );
      task->insertMetaAttribute( TaskMetaHash, hashForFile( fileName ) );
    }
  }
}

/**
   @brief Ensure the todo list is compatible with 0.2 app version
   @todo Remove this in 0.3 release
 */
void LocalXmlBackend::fixTodoList(const QString &todoList)
{
  QDomDocument doc = documentForFile( todoList );
  QDomElement root = doc.documentElement();
  if ( !root.hasAttribute( "id" ) ) {
    root.setAttribute( "id", QUuid::createUuid().toString() );
    documentToFile( doc, todoList );
    qDebug() << "Todo list" << todoList << "updated!";
  }
}

/**
   @brief Ensure the todo is compatible with 0.2 app version
   @todo Remove this in 0.3 release
 */
void LocalXmlBackend::fixTodo(const QString &todo)
{
  QDomDocument doc = documentForFile( todo );
  QDomElement root = doc.documentElement();
  bool changed = false;
  if ( !root.hasAttribute( "id" ) ) {
    root.setAttribute( "id", QUuid::createUuid().toString() );
    changed = true;
  }
  if ( !root.hasAttribute( "done" ) ) {
    root.setAttribute( "done", root.attribute( "progress", "0" ).toInt() == 100 ? "true" : "false" );
    changed = true;
  }
  if ( !root.hasAttribute( "weight" ) ) {
    root.setAttribute( "weight",  ( qrand() % 10000 / 100.0 )  );
    changed = true;
  }
  if ( changed ) {
    documentToFile( doc, todo );
    qDebug() << "Todo" << todo << "updated!";
  }
}

QDomDocument LocalXmlBackend::documentForFile(const QString &fileName) const
{
  QString fullName = m_localStorageDirectory + "/" + fileName;
  QFile file( fullName );
  if ( file.exists() ) {
    if ( file.open( QIODevice::ReadOnly ) ) {
      QDomDocument doc;
      QString errorMsg;
      int errorLine, errorColumn;
      if ( doc.setContent(  &file, &errorMsg, &errorLine, &errorColumn ) ) {
        file.close();
        return doc;
      } else {
        qWarning() << "Error reading XML document:" << errorMsg << "in line" << errorLine
                   << "in column" << errorColumn;
      }
      file.close();
    } else {
      qWarning() << "Unable to open" << file.fileName() << "for reading!";
    }
  }
  return QDomDocument();
}

void LocalXmlBackend::documentToFile(const QDomDocument &doc, const QString &fileName) const
{
  QString fullName = m_localStorageDirectory + "/" + fileName;
  QFile file( fullName );
  if ( file.open( QIODevice::WriteOnly ) ) {
    file.write( doc.toByteArray( 2 ) );
    file.close();
  } else {
    qWarning() << "Failed to open file" << fullName << "for writing:" << file.errorString();
  }
}

QByteArray LocalXmlBackend::hashForFile(const QString &fileName) const
{
  QString fullName = m_localStorageDirectory + "/" + fileName;
  QFile file( fullName );
  if ( file.open( QIODevice::ReadOnly ) ) {
    QCryptographicHash hash( QCryptographicHash::Sha3_512 );
    hash.addData( &file );
    file.close();
    return hash.result().toHex();
  }
  return QByteArray();
}

bool LocalXmlBackend::todoListNeedsUpdate(ITodoList *todoList, const QString &fileName, QByteArray &hash) const
{
  bool result = true;
  hash = hashForFile( fileName );
  ITodoList *existingTodoList = m_database->getTodoList( todoList->uuid() );
  if ( existingTodoList ) {
    result = hash != existingTodoList->metaAttributes().value( TodoListMetaHash ).toByteArray();
    delete existingTodoList;
  }
  return result;
}

bool LocalXmlBackend::todoNeedsUpdate(ITodo *todo, const QString &fileName, QByteArray &hash) const
{
  bool result = true;
  hash = hashForFile( fileName );
  ITodo *existingTodo = m_database->getTodo( todo->uuid() );
  if ( existingTodo ) {
    result = existingTodo->metaAttributes().value( TodoMetaHash ).toByteArray() != hash;
    delete existingTodo;
  }
  return result;
}

bool LocalXmlBackend::taskNeedsUpdate(ITask *task, const QString &fileName, QByteArray &hash) const
{
  bool result = true;
  hash = hashForFile( fileName );
  ITask *existingTask = m_database->getTask( task->uuid() );
  if ( existingTask ) {
    result = existingTask->metaAttributes().value( TaskMetaHash ).toByteArray() != hash;
    delete existingTask;
  }
  return result;
}

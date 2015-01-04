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

#include "backendwrapper.h"

#include "datamodel/account.h"
#include "datamodel/todolist.h"
#include "datamodel/todo.h"
#include "datamodel/task.h"

#include "database/database.h"

#include "database/queries/deleteaccount.h"
#include "database/queries/deletetask.h"
#include "database/queries/deletetodo.h"
#include "database/queries/deletetodolist.h"
#include "database/queries/insertaccount.h"
#include "database/queries/inserttodolist.h"
#include "database/queries/inserttodo.h"
#include "database/queries/inserttask.h"
#include "database/queries/readaccount.h"
#include "database/queries/readtask.h"
#include "database/queries/readtodo.h"
#include "database/queries/readtodolist.h"
#include "database/queries/saveaccount.h"
#include "database/queries/savetask.h"
#include "database/queries/savetodo.h"
#include "database/queries/savetodolist.h"

#include <QDebug>

namespace OpenTodoList {

namespace DataBase {

BackendWrapper::BackendWrapper(QObject *parent) :
  QObject( parent ),
  m_database( 0 ),
  m_backend( 0 ),
  m_status( Invalid ),
  m_syncTimer( nullptr )
{
}

BackendWrapper::BackendWrapper(Database *database, IBackend *backend, QObject *parent) :
  QObject(parent),
  m_database( database ),
  m_backend( backend ),
  m_status( Stopped )
{
  Q_ASSERT( m_database );
  Q_ASSERT( m_backend );

  m_backend->setDatabase( this );
}

BackendWrapper::~BackendWrapper()
{
  // Don't do this! The actual backend is released when the plugin's
  // QObject instance is destroyed!
  //delete m_backend;
}

bool BackendWrapper::insertAccount(IAccount *account)
{
  DataModel::Account *acc = static_cast< DataModel::Account* >( account );
  Queries::InsertAccount q( acc, false );
  m_database->runQuery( &q );
  return true;
}

bool BackendWrapper::insertTodoList(ITodoList *list)
{
  DataModel::TodoList *todoList = static_cast< DataModel::TodoList* >( list );
  Queries::InsertTodoList q( todoList, false );
  m_database->runQuery( &q );
  return true;
}

bool BackendWrapper::insertTodo(ITodo *todo)
{
  DataModel::Todo *t = static_cast< DataModel::Todo* >( todo );
  Queries::InsertTodo q( t, false );
  m_database->runQuery( &q );
  return true;
}

bool BackendWrapper::insertTask(ITask *task)
{
  DataModel::Task *t = static_cast< DataModel::Task* >( task );
  Queries::InsertTask q( t, false );
  m_database->runQuery( &q );
  return true;
}

bool BackendWrapper::deleteAccount(IAccount *account)
{
  DataModel::Account *t = static_cast<DataModel::Account*>( account );
  Queries::DeleteAccount q( t );
  m_database->runQuery( &q );
  return true;
}

bool BackendWrapper::deleteTodoList(ITodoList *list)
{
  DataModel::TodoList *t = static_cast<DataModel::TodoList*>( list );
  Queries::DeleteTodoList q( t );
  m_database->runQuery( &q );
  return true;
}

bool BackendWrapper::deleteTodo(ITodo *todo)
{
  DataModel::Todo *t = static_cast<DataModel::Todo*>( todo );
  Queries::DeleteTodo q( t );
  m_database->runQuery( &q );
  return true;
}

bool BackendWrapper::deleteTask(ITask *task)
{
  DataModel::Task *t = static_cast<DataModel::Task*>( task );
  Queries::DeleteTask q( t );
  m_database->runQuery( &q );
  return true;
}

IAccount *BackendWrapper::createAccount()
{
  DataModel::Account *account = new DataModel::Account();
  account->setBackend( name() ); // inject our name
  return account;
}

ITodoList *BackendWrapper::createTodoList()
{
  return new DataModel::TodoList();
}

ITodo *BackendWrapper::createTodo()
{
  return new DataModel::Todo();
}

ITask *BackendWrapper::createTask()
{
  return new DataModel::Task();
}

IAccount *BackendWrapper::getAccount(const QUuid &uuid)
{
  Queries::ReadAccount q;
  q.setUuid( uuid );
  m_database->runQuery( &q );
  if ( q.objects().isEmpty() ) {
    return nullptr;
  } else {
    IAccount *result = createAccount();
    static_cast<DataModel::Account*>(result)->fromVariant( q.objects().first()->toVariant());
    return result;
  }
}

ITodoList *BackendWrapper::getTodoList(const QUuid &uuid)
{
  Queries::ReadTodoList q;
  q.setUuid( uuid );
  m_database->runQuery( &q );
  if ( q.objects().isEmpty() ) {
    return nullptr;
  } else {
    ITodoList *result = createTodoList();
    static_cast<DataModel::TodoList*>(result)->fromVariant( q.objects().first()->toVariant());
    return result;
  }
}

ITodo *BackendWrapper::getTodo(const QUuid &uuid)
{
  Queries::ReadTodo q;
  q.setUuid( uuid );
  m_database->runQuery( &q );
  if ( q.objects().isEmpty() ) {
    return nullptr;
  } else {
    ITodo *result = createTodo();
    static_cast<DataModel::Todo*>(result)->fromVariant( q.objects().first()->toVariant());
    return result;
  }
}

ITask *BackendWrapper::getTask(const QUuid &uuid)
{
  Queries::ReadTask q;
  q.setUuid( uuid );
  m_database->runQuery( &q );
  if ( q.objects().isEmpty() ) {
    return nullptr;
  } else {
    ITask *result = createTask();
    static_cast<DataModel::Task*>(result)->fromVariant( q.objects().first()->toVariant());
    return result;
  }
}

QList<IAccount *> BackendWrapper::getAccounts(QueryFlags flags, int maxAccounts, int offset)
{
  Queries::ReadAccount q;
  if ( flags & QueryDirty ) {
    q.setOnlyModified( true );
  }
  if ( flags & QueryDisposed ) {
    q.setOnlyDeleted( true );
  }
  q.setLimit( maxAccounts );
  q.setOffset( offset );
  Queries::ReadAccount::Condition c;
  c.condition = "backend.name=:searchBackendName";
  c.arguments.insert( "searchBackendName", m_backend->name() );
  q.addCondition(c);
  m_database->runQuery(&q);
  QList<IAccount*> result;
  for ( DataModel::Account *account : q.objects() ) {
    IAccount *a = createAccount();
    static_cast<DataModel::Account*>( a )->fromVariant( account->toVariant() );
    result << a;
  }
  return result;
}

QList<ITodoList *> BackendWrapper::getTodoLists(QueryFlags flags, int maxTodoLists, int offset)
{
  Queries::ReadTodoList q;
  if ( flags & QueryDirty ) {
    q.setOnlyModified( true );
  }
  if ( flags & QueryDisposed ) {
    q.setOnlyDeleted( true );
  }
  q.setLimit( maxTodoLists );
  q.setOffset( offset );
  Queries::ReadTodoList::Condition c;
  c.condition = "backend.name=:searchBackendName";
  c.arguments.insert( "searchBackendName", m_backend->name() );
  q.addCondition(c);
  m_database->runQuery(&q);
  QList<ITodoList*> result;
  for ( DataModel::TodoList *todoList : q.objects() ) {
    ITodoList *tl = createTodoList();
    static_cast<DataModel::TodoList*>( tl )->fromVariant( todoList->toVariant() );
    result << tl;
  }
  return result;
}

QList<ITodo *> BackendWrapper::getTodos(QueryFlags flags, int maxTodos, int offset)
{
  Queries::ReadTodo q;
  if ( flags & QueryDirty ) {
    q.setOnlyModified( true );
  }
  if ( flags & QueryDisposed ) {
    q.setOnlyDeleted( true );
  }
  q.setLimit( maxTodos );
  q.setOffset( offset );
  Queries::ReadTodo::Condition c;
  c.condition = "backend.name=:searchBackendName";
  c.arguments.insert( "searchBackendName", m_backend->name() );
  q.addCondition(c);
  m_database->runQuery(&q);
  QList<ITodo*> result;
  for ( DataModel::Todo *todo : q.objects() ) {
    ITodo *t = createTodo();
    static_cast<DataModel::Todo*>( t )->fromVariant( todo->toVariant() );
    result << t;
  }
  return result;
}

QList<ITask *> BackendWrapper::getTasks(QueryFlags flags, int maxTasks, int offset)
{
  Queries::ReadTask q;
  if ( flags & QueryDirty ) {
    q.setOnlyModified( true );
  }
  if ( flags & QueryDisposed ) {
    q.setOnlyDeleted( true );
  }
  q.setLimit( maxTasks );
  q.setOffset( offset );
  Queries::ReadTask::Condition c;
  c.condition = "backend.name=:searchBackendName";
  c.arguments.insert( "searchBackendName", m_backend->name() );
  q.addCondition(c);
  m_database->runQuery(&q);
  QList<ITask*> result;
  for ( DataModel::Task *task : q.objects() ) {
    ITask *t = createTask();
    static_cast<DataModel::Task*>( t )->fromVariant( task->toVariant() );
    result << t;
  }
  return result;
}

bool BackendWrapper::onAccountSaved(IAccount *account)
{
  DataModel::Account *tmp = static_cast< DataModel::Account* >( account );
  Queries::SaveAccount q( tmp );
  m_database->runQuery( &q );
  return true;
}

bool BackendWrapper::onTodoListSaved(ITodoList *todoList)
{
  DataModel::TodoList *tmp = static_cast< DataModel::TodoList* >( todoList );
  Queries::SaveTodoList q( tmp );
  m_database->runQuery( &q );
  return true;
}

bool BackendWrapper::onTodoSaved(ITodo *todo)
{
  DataModel::Todo *tmp = static_cast< DataModel::Todo* >( todo );
  Queries::SaveTodo q( tmp );
  m_database->runQuery( &q );
  return true;
}

bool BackendWrapper::onTaskSaved(ITask *task)
{
  DataModel::Task *tmp = static_cast< DataModel::Task* >( task );
  Queries::SaveTask q( tmp );
  m_database->runQuery( &q );
  return true;
}

void BackendWrapper::setLocalStorageDirectory(const QString &directory)
{
  if ( m_status != Invalid )
    m_backend->setLocalStorageDirectory( directory );
}

QString BackendWrapper::name() const
{
  if ( m_status != Invalid )
    return m_backend->name();
  return QString();
}

QString BackendWrapper::title() const
{
  if ( m_status != Invalid )
    return m_backend->title();
  return QString();
}

QString BackendWrapper::description() const
{
  if ( m_status != Invalid )
    return m_backend->description();
  return QString();
}

bool BackendWrapper::start()
{
  switch ( m_status ) {
  case Invalid: return false;
  case Running: return true;
  case Stopped:
    if ( m_backend->start() ) {
      setStatus( Running );
      m_syncTimer = new QTimer( this );
      // TODO: Look for more intelligent ways of syncing
      m_syncTimer->setInterval( 10 * 60 * 1000 ); // Save every 10 minutes
      m_syncTimer->setSingleShot( true );
      connect( m_syncTimer, &QTimer::timeout, [this] { this->sync(); } );
      m_syncTimer->start();
      return true;
    } else {
      return false;
    }
  }
  qWarning() << "Unhandled status in BackendWrapper::start():" << m_status;
  return false;
}

bool BackendWrapper::stop()
{
  switch ( m_status ) {
  case Invalid: return false;
  case Stopped: return true;
  case Running:
    delete m_syncTimer;
    m_syncTimer = nullptr;
    if ( m_backend->stop() ) {
      setStatus( Stopped );
      return true;
    } else {
      return false;
    }
  }
  qWarning() << "Unhandled status in BackendWrapper::stop():" << m_status;
  return false;
}

void BackendWrapper::sync()
{
  qDebug() << "Sync tick in backend" << m_backend->name() << "started";
  m_backend->sync();
  m_syncTimer->start();
  qDebug() << "Sync tick in backend" << m_backend->name() << "finished";
}

void BackendWrapper::doStart()
{
  if ( !start() ) {
    qWarning() << "Failed to start backend" << title();
  } else {
    qDebug() << "Started backend" << title();
  }
}

void BackendWrapper::doStop()
{
  if ( !stop() ) {
    qWarning() << "Failed to stop backend" << title();
  } else {
    qDebug() << "Stopped backend" << title();
  }
}

IBackend *BackendWrapper::backend() const
{
  return m_backend;
}

void BackendWrapper::setBackend(IBackend *backend)
{
  m_backend = backend;
}


void BackendWrapper::setDatabase(IDatabase *database)
{
  Q_ASSERT( false );
  Q_UNUSED( database );
}

void BackendWrapper::setStatus(BackendWrapper::Status newStatus)
{
  if ( m_status != newStatus ) {
    m_status = newStatus;
    emit statusChanged();
  }
}

} /* DataBase */

} /* OpenTodoList */

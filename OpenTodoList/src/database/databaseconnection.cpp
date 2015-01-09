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

#include "databaseconnection.h"

#include "database/queries/disposeaccount.h"
#include "database/queries/disposetask.h"
#include "database/queries/disposetodo.h"
#include "database/queries/disposetodolist.h"
#include "database/queries/insertaccount.h"
#include "database/queries/inserttodolist.h"
#include "database/queries/inserttodo.h"
#include "database/queries/inserttask.h"

namespace OpenTodoList {
namespace DataBase {

DatabaseConnection::DatabaseConnection(QObject *parent) :
  QObject(parent),
  m_database( nullptr ),
  m_registeredObjects()
{

}

DatabaseConnection::~DatabaseConnection()
{

}

/**
   @brief The database the connection works on

   The connection will work with this database object when inserting or removing objects
   is requested.

   @sa setDatabase()
 */
Database *DatabaseConnection::database() const
{
  return m_database;
}

/**
   @brief Sets the database

   @sa database()
 */
void DatabaseConnection::setDatabase(Database *database)
{
  if ( m_database != database ) {
    if ( m_database ) {
      disconnect( m_database, &Database::accountChanged,
               this, &DatabaseConnection::onDatabaseAccountUpdate );
      disconnect( m_database, &Database::todoListChanged,
               this, &DatabaseConnection::onDatabaseTodoListUpdate );
      disconnect( m_database, &Database::todoChanged,
               this, &DatabaseConnection::onDatabaseTodoUpdate );
      disconnect( m_database, &Database::taskChanged,
               this, &DatabaseConnection::onDatabaseTaskUpdate );
    }
    m_database = database;
    if ( m_database ) {
      connect( m_database, &Database::accountChanged,
               this, &DatabaseConnection::onDatabaseAccountUpdate );
      connect( m_database, &Database::todoListChanged,
               this, &DatabaseConnection::onDatabaseTodoListUpdate );
      connect( m_database, &Database::todoChanged,
               this, &DatabaseConnection::onDatabaseTodoUpdate );
      connect( m_database, &Database::taskChanged,
               this, &DatabaseConnection::onDatabaseTaskUpdate );
    }
    emit databaseChanged();
  }
}

/**
   @brief Writes the @p account into the database

   @note If the UUID of the object is null, a new UUID will be generated.
 */
void DatabaseConnection::insertAccount(Account *account)
{
  Q_ASSERT( account );
  if ( m_database != nullptr ) {
    if ( account->uuid().isNull() ) {
      account->setUuid( QUuid::createUuid() );
    }
    auto tmp = new Account();
    tmp->fromVariant( account->toVariant() );
    auto q = new Queries::InsertAccount( tmp, true );
    tmp->setParent( q );
    m_database->scheduleQuery( q );
  }
}

/**
   @brief Writes the @p todoList into the database

   @note If the UUID of the object is null, a new UUID will be generated.
 */
void DatabaseConnection::insertTodoList(TodoList *todoList)
{
  Q_ASSERT( todoList );
  if ( m_database != nullptr ) {
    if ( todoList->uuid().isNull() ) {
      todoList->setUuid( QUuid::createUuid() );
    }
    auto tmp = new TodoList();
    tmp->fromVariant( todoList->toVariant() );
    auto q = new Queries::InsertTodoList( tmp, true );
    tmp->setParent( q );
    m_database->scheduleQuery( q );
  }
}

/**
   @brief Writes the @p todo into the database

   @note If the UUID of the object is null, a new UUID will be generated.
 */
void DatabaseConnection::insertTodo(Todo *todo)
{
  Q_ASSERT( todo );
  if ( m_database != nullptr ) {
    if ( todo->uuid().isNull() ) {
      todo->setUuid( QUuid::createUuid() );
    }
    auto tmp = new Todo();
    tmp->fromVariant( todo->toVariant() );
    auto q = new Queries::InsertTodo( tmp, true );
    tmp->setParent( q );
    m_database->scheduleQuery( q );
  }
}

/**
   @brief Writes the @p task into the database

   @note If the UUID of the object is null, a new UUID will be generated.
 */
void DatabaseConnection::insertTask(Task *task)
{
  Q_ASSERT( task );
  if ( m_database != nullptr ) {
    if ( task->uuid().isNull() ) {
      task->setUuid( QUuid::createUuid() );
    }
    auto tmp = new Task();
    tmp->fromVariant( task->toVariant() );
    auto q = new Queries::InsertTask( tmp, true );
    tmp->setParent( q );
    m_database->scheduleQuery( q );
  }
}

void DatabaseConnection::disposeAccount(Account *account)
{
  Q_ASSERT( account );
  if ( m_database ) {
    auto tmp = new Account();
    tmp->fromVariant( account->toVariant() );
    auto q = new Queries::DisposeAccount( tmp );
    tmp->setParent( q );
    m_database->scheduleQuery( q );
  }
}

void DatabaseConnection::disposeTodoList(TodoList *todoList)
{
  Q_ASSERT( todoList );
  if ( m_database ) {
    auto tmp = new TodoList();
    tmp->fromVariant( todoList->toVariant() );
    auto q = new Queries::DisposeTodoList( tmp );
    tmp->setParent( q );
    m_database->scheduleQuery( q );
  }
}

void DatabaseConnection::disposeTodo(Todo *todo)
{
  Q_ASSERT( todo );
  if ( m_database ) {
    auto tmp = new Todo();
    tmp->fromVariant( todo->toVariant() );
    auto q = new Queries::DisposeTodo( tmp );
    tmp->setParent( q );
    m_database->scheduleQuery( q );
  }
}

void DatabaseConnection::disposeTask(Task *task)
{
  Q_ASSERT( task );
  if ( m_database ) {
    auto tmp = new Task();
    tmp->fromVariant( task->toVariant() );
    auto q = new Queries::DisposeTask( tmp );
    tmp->setParent( q );
    m_database->scheduleQuery( q );
  }
}

/**
   @brief Registers the account

   This will register the @p account for updates to or from the
   database, depending on the @p updateFlags.
 */
void DatabaseConnection::registerAccount(Account *account, DatabaseConnection::UpdateFlags updateFlags)
{
  registerObjectImpl( account, updateFlags );
}

/**
   @brief Registers the todo list

   This will register the @p todoList for updates to or from the
   database, depending on the @p updateFlags.
 */
void DatabaseConnection::registerTodoList(TodoList *todoList, DatabaseConnection::UpdateFlags updateFlags)
{
  registerObjectImpl( todoList, updateFlags );
}

/**
   @brief Registers the todo

   This will register the @p todo for updates to or from the
   database, depending on the @p updateFlags.
 */
void DatabaseConnection::registerTodo(Todo *todo, DatabaseConnection::UpdateFlags updateFlags)
{
  registerObjectImpl( todo, updateFlags );
}

/**
   @brief Registers the task

   This will register the @p task for updates to or from the
   database, depending on the @p updateFlags.
 */
void DatabaseConnection::registerTask(Task *task, DatabaseConnection::UpdateFlags updateFlags)
{
  registerObjectImpl( task, updateFlags );
}

void DatabaseConnection::onObjectInstanceUpdate()
{
  if ( m_database ) {
    if ( m_registeredObjects.value( sender() ) & ObjectToDatabase ) {
      Account *account = qobject_cast<Account*>(sender());
      if ( account ) {
        insertAccount(account);
        return;
      }
      TodoList *todoList = qobject_cast<TodoList*>(sender());
      if ( todoList ) {
        insertTodoList(todoList);
        return;
      }
      Todo *todo = qobject_cast<Todo*>(sender());
      if ( todo ) {
        insertTodo(todo);
        return;
      }
      Task *task = qobject_cast<Task*>(sender());
      if ( task ) {
        insertTask(task);
        return;
      }
    }
  }
}

void DatabaseConnection::onDatabaseAccountUpdate(const QVariant &account)
{
  onDatabaseUpdateImpl<Account>(account);
}

void DatabaseConnection::onDatabaseTodoListUpdate(const QVariant &todoList)
{
  onDatabaseUpdateImpl<TodoList>(todoList);
}

void DatabaseConnection::onDatabaseTodoUpdate(const QVariant &todo)
{
  onDatabaseUpdateImpl<Todo>(todo);
}

void DatabaseConnection::onDatabaseTaskUpdate(const QVariant &task)
{
  onDatabaseUpdateImpl<Task>(task);
}

void DatabaseConnection::onObjectDestroyed(QObject *o)
{
  m_registeredObjects.remove(o);
}

} // namespace DataBase
} // namespace OpenTodoList


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

DatabaseConnection::DatabaseConnection(QObject *parent) : QObject(parent)
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
    m_database = database;
    emit databaseChanged();
  }
}

/**
   @brief Writes the @p account into the database
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


} // namespace DataBase
} // namespace OpenTodoList


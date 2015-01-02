#ifndef OPENTODOLIST_DATABASE_DATABASECONNECTION_H
#define OPENTODOLIST_DATABASE_DATABASECONNECTION_H

#include "datamodel/account.h"
#include "datamodel/todolist.h"
#include "datamodel/todo.h"
#include "datamodel/task.h"

#include "database/database.h"

namespace OpenTodoList {
namespace DataBase {

using namespace DataModel;

/**
   @brief A connection to the database

   This is a helper class that allows accessing the database from the front end part
   and insert, modify and dispose objects from it.
 */
class DatabaseConnection : public QObject
{
  Q_OBJECT
  Q_PROPERTY(OpenTodoList::DataBase::Database* database READ database WRITE setDatabase NOTIFY databaseChanged)
public:
  explicit DatabaseConnection(QObject *parent = 0);
  ~DatabaseConnection();

  Database *database() const;
  void setDatabase(Database *database);

signals:

  void databaseChanged();

public slots:

  void insertAccount( Account *account );
  void insertTodoList( TodoList *todoList );
  void insertTodo( Todo *todo );
  void insertTask( Task *task );
  void disposeAccount( Account *account );
  void disposeTodoList( TodoList *todoList );
  void disposeTodo( Todo *todo );
  void disposeTask( Task *task );

private:

  Database *m_database;
};

} // namespace DataBase
} // namespace OpenTodoList

#endif // OPENTODOLIST_DATABASE_DATABASECONNECTION_H

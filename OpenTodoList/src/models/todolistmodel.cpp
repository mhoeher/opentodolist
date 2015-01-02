#include "models/todolistmodel.h"

#include "datamodel/objectinfo.h"

#include "database/queries/readtodolist.h"
#include "database/databaseconnection.h"

#include <QDebug>
#include <QTimer>

namespace OpenTodoList {

namespace Models {

TodoListModel::TodoListModel(QObject *parent) :
    ObjectModel( ObjectInfo<TodoList>::classUuidProperty(), parent )
{
  connect( this, &TodoListModel::objectAdded, [this] (QObject *object) {
    TodoList *todoList = dynamic_cast< TodoList* >( object );
    if ( todoList ) {
      this->connect( todoList, &TodoList::changed, [todoList,this] {
        if ( this->database() ) {
          DatabaseConnection conn;
          conn.setDatabase( this->database() );
          conn.insertTodoList( todoList );
        }
      });
    }
  });
}

TodoListModel::~TodoListModel()
{
}

void TodoListModel::connectToDatabase()
{
  connect( database(), &Database::todoListChanged, this, &TodoListModel::addTodoList );
  connect( database(), &Database::todoListDeleted, this, &TodoListModel::removeTodoList );
}

void TodoListModel::disconnectFromDatabase()
{
  disconnect( database(), &Database::todoListChanged, this, &TodoListModel::addTodoList );
  disconnect( database(), &Database::todoListDeleted, this, &TodoListModel::removeTodoList );
}

StorageQuery *TodoListModel::createQuery() const
{
  Queries::ReadTodoList *q = new Queries::ReadTodoList();
  connect( q, &Queries::ReadTodoList::readTodoList,
           this, &TodoListModel::addTodoList, Qt::QueuedConnection );
  return q;
}

int TodoListModel::compareObjects(QObject *left, QObject *right) const
{
  TodoList *leftTodoList = dynamic_cast< TodoList* >( left );
  TodoList *rightTodoList = dynamic_cast< TodoList* >( right );
  if ( leftTodoList && rightTodoList ) {
    return leftTodoList->name().localeAwareCompare( rightTodoList->name() );
  }
  return 0;
}

void TodoListModel::addTodoList(const QVariant &todoList)
{
  addObject<TodoList>( todoList );
}

void TodoListModel::removeTodoList(const QVariant &todoList)
{
  removeObject<TodoList>( todoList );
}

} /* Models */

} /* OpenTodoList */

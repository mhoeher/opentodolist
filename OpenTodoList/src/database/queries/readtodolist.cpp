#include "readtodolist.h"

namespace OpenTodoList {
namespace DataBase {
namespace Queries {

ReadTodoList::ReadTodoList() :
    ReadObject<TodoList>( { "uuid", "name" } )
{
  connect( this, &ReadTodoList::queryFinished,
           [this] {
    for ( TodoList *todoList : this->objects() ) {
      emit this->readTodoList( todoList->toVariant() );
    }
  });
}

QList<TodoList *> ReadTodoList::todoLists() const
{
    return objects();
}

} // namespace Queries
} // namespace DataBase
} // namespace OpenTodoList

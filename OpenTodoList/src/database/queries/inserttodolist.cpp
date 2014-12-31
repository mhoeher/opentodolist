#include "inserttodolist.h"

namespace OpenTodoList {
namespace DataBase {
namespace Queries {

InsertTodoList::InsertTodoList( TodoList *todoList, bool update ) :
    InsertObject<TodoList>(
      todoList,
      { "uuid", "name" },
      update )
{
  connect( this, &InsertTodoList::queryFinished,
           [this,todoList] { emit this->todoListChanged( todoList->toVariant() ) ; } );
}

} // namespace Queries
} // namespace DataBase
} // namespace OpenTodoList

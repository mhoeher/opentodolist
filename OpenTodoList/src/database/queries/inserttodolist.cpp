#include "inserttodolist.h"

namespace OpenTodoList {
namespace DataBase {
namespace Queries {

InsertTodoList::InsertTodoList( TodoList *todoList, bool update ) :
    InsertObject<TodoList>(
      todoList,
      "todoList",
      "todoListMetaAttributeName",
      "todoListMetaAttribute",
      { "uuid", "name" },
      update,
      [this,todoList] { emit this->todoListChanged( todoList->toVariant() ); },
      "account",
      "uuid" )
{
}

} // namespace Queries
} // namespace DataBase
} // namespace OpenTodoList

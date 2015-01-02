#include "savetodolist.h"

namespace OpenTodoList {
namespace DataBase {
namespace Queries {

SaveTodoList::SaveTodoList(TodoList *todoList) :
  SaveObject<TodoList>( todoList )
{
}

SaveTodoList::~SaveTodoList()
{
}

} // namespace Queries
} // namespace DataBase
} // namespace OpenTodoList


#include "inserttodo.h"

#include "QTextStream"

namespace OpenTodoList {
namespace DataBase {
namespace Queries {

InsertTodo::InsertTodo(Todo *todo, bool update ) :
  InsertObject<Todo>(
    todo,
    "todo",
    "todoMetaAttributeName",
    "todoMetaAttribute",
    { "uuid", "weight", "done", "priority", "dueDate", "title", "description" },
    update,
    [this,todo] { emit this->todoChanged( todo->toVariant() ); },
    "todoList",
    "uuid" )
{
}

} // namespace Queries
} // namespace DataBase
} // namespace OpenTodoList

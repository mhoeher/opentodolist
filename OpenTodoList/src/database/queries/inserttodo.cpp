#include "inserttodo.h"

#include "QTextStream"

namespace OpenTodoList {
namespace DataBase {
namespace Queries {

InsertTodo::InsertTodo(Todo *todo, bool update ) :
  InsertObject<Todo>(
    todo,
    { "uuid", "weight", "done", "priority", "dueDate", "title", "description" },
    update )
{
  connect( this, &InsertTodo::queryFinished,
           [this,todo] { emit this->todoChanged( todo->toVariant() ); } );
}

} // namespace Queries
} // namespace DataBase
} // namespace OpenTodoList

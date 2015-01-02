#include "savetodo.h"

namespace OpenTodoList {
namespace DataBase {
namespace Queries {

SaveTodo::SaveTodo(Todo *todo) :
  SaveObject<Todo>( todo )
{

}

SaveTodo::~SaveTodo()
{

}

} // namespace Queries
} // namespace DataBase
} // namespace OpenTodoList


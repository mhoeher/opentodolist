#include "deletetodo.h"

namespace OpenTodoList {
namespace DataBase {
namespace Queries {

DeleteTodo::DeleteTodo(Todo *todo) :
  DeleteObject< Todo >( todo )
{
  connect( this, &DeleteTodo::queryFinished,
           [this,todo] {
    emit this->todoDeleted( todo->toVariant() );
  });
}

DeleteTodo::~DeleteTodo()
{

}

} // namespace Queries
} // namespace DataBase
} // namespace OpenTodoList


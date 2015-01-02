#include "disposetodo.h"

namespace OpenTodoList {
namespace DataBase {
namespace Queries {

DisposeTodo::DisposeTodo(Todo *todo) :
  DisposeObject<Todo>( todo )
{
  connect( this, &DisposeTodo::queryFinished,
           [this,todo] {
    todo->setDisposed( true );
    emit this->todoChanged( todo->toVariant() );
  });
}

DisposeTodo::~DisposeTodo()
{

}

} // namespace Queries
} // namespace DataBase
} // namespace OpenTodoList


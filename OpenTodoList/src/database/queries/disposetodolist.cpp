#include "disposetodolist.h"

namespace OpenTodoList {
namespace DataBase {
namespace Queries {

DisposeTodoList::DisposeTodoList(TodoList *todoList) :
  DisposeObject<TodoList>( todoList )
{
  connect( this, &DisposeTodoList::queryFinished,
           [this,todoList] {
    todoList->setDisposed(true);
    emit this->todoListChanged( todoList->toVariant() );
  });
}

DisposeTodoList::~DisposeTodoList()
{

}

} // namespace Queries
} // namespace DataBase
} // namespace OpenTodoList


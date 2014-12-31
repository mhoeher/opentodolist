#include "deletetodolist.h"

namespace OpenTodoList {
namespace DataBase {
namespace Queries {

DeleteTodoList::DeleteTodoList(TodoList *todoList) :
  DeleteObject<TodoList>( todoList )
{
  connect( this, &DeleteTodoList::queryFinished,
           [this,todoList] {
    emit this->todoListDeleted( todoList->toVariant() );
  });
}

DeleteTodoList::~DeleteTodoList()
{

}

} // namespace Queries
} // namespace DataBase
} // namespace OpenTodoList


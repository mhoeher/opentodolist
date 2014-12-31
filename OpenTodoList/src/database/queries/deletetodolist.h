#ifndef OPENTODOLIST_DATABASE_QUERIES_DELETETODOLIST_H
#define OPENTODOLIST_DATABASE_QUERIES_DELETETODOLIST_H

#include "datamodel/todolist.h"
#include "database/queries/private/deleteobject.h"

namespace OpenTodoList {
namespace DataBase {
namespace Queries {

using namespace Private;
using namespace DataModel;

class DeleteTodoList : public DeleteObject<TodoList>
{
public:
  DeleteTodoList( TodoList *todoList );
  ~DeleteTodoList();
};

} // namespace Queries
} // namespace DataBase
} // namespace OpenTodoList

#endif // OPENTODOLIST_DATABASE_QUERIES_DELETETODOLIST_H

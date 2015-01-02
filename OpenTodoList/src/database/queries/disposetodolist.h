#ifndef OPENTODOLIST_DATABASE_QUERIES_DISPOSETODOLIST_H
#define OPENTODOLIST_DATABASE_QUERIES_DISPOSETODOLIST_H

#include "datamodel/todolist.h"
#include "database/queries/private/disposeobject.h"

namespace OpenTodoList {
namespace DataBase {
namespace Queries {

using namespace Private;
using namespace DataModel;

class DisposeTodoList : public DisposeObject<TodoList>
{
public:
  DisposeTodoList( TodoList *todoList );
  ~DisposeTodoList();
};

} // namespace Queries
} // namespace DataBase
} // namespace OpenTodoList

#endif // OPENTODOLIST_DATABASE_QUERIES_DISPOSETODOLIST_H

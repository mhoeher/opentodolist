#ifndef OPENTODOLIST_DATABASE_QUERIES_SAVETODOLIST_H
#define OPENTODOLIST_DATABASE_QUERIES_SAVETODOLIST_H

#include "datamodel/todolist.h"
#include "database/queries/private/saveobject.h"

namespace OpenTodoList {
namespace DataBase {
namespace Queries {

using namespace Private;
using namespace DataModel;

class SaveTodoList : public SaveObject<TodoList>
{
public:
  SaveTodoList( TodoList *todoList );
  ~SaveTodoList();
};

} // namespace Queries
} // namespace DataBase
} // namespace OpenTodoList

#endif // OPENTODOLIST_DATABASE_QUERIES_SAVETODOLIST_H

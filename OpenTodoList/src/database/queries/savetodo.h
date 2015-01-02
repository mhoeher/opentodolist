#ifndef OPENTODOLIST_DATABASE_QUERIES_SAVETODO_H
#define OPENTODOLIST_DATABASE_QUERIES_SAVETODO_H

#include "datamodel/todo.h"
#include "database/queries/private/saveobject.h"

namespace OpenTodoList {
namespace DataBase {
namespace Queries {

using namespace Private;
using namespace DataModel;

class SaveTodo : public SaveObject<Todo>
{
public:
  SaveTodo( Todo *todo );
  ~SaveTodo();
};

} // namespace Queries
} // namespace DataBase
} // namespace OpenTodoList

#endif // OPENTODOLIST_DATABASE_QUERIES_SAVETODO_H

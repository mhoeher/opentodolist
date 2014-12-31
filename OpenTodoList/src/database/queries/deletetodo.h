#ifndef OPENTODOLIST_DATABASE_QUERIES_DELETETODO_H
#define OPENTODOLIST_DATABASE_QUERIES_DELETETODO_H

#include "datamodel/todo.h"
#include "database/queries/private/deleteobject.h"

namespace OpenTodoList {
namespace DataBase {
namespace Queries {

using namespace Private;
using namespace DataModel;

class DeleteTodo : public DeleteObject<Todo>
{
public:
  DeleteTodo( Todo *todo );
  ~DeleteTodo();
};

} // namespace Queries
} // namespace DataBase
} // namespace OpenTodoList

#endif // OPENTODOLIST_DATABASE_QUERIES_DELETETODO_H

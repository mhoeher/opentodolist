#ifndef OPENTODOLIST_DATABASE_QUERIES_DISPOSETODO_H
#define OPENTODOLIST_DATABASE_QUERIES_DISPOSETODO_H

#include "datamodel/todo.h"
#include "database/queries/private/disposeobject.h"

namespace OpenTodoList {
namespace DataBase {
namespace Queries {

using namespace Private;
using namespace DataModel;

class DisposeTodo : public DisposeObject<Todo>
{
public:
  DisposeTodo( Todo *todo );
  ~DisposeTodo();
};

} // namespace Queries
} // namespace DataBase
} // namespace OpenTodoList

#endif // OPENTODOLIST_DATABASE_QUERIES_DISPOSETODO_H

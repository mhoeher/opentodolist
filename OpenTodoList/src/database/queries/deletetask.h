#ifndef OPENTODOLIST_DATABASE_QUERIES_DELETETASK_H
#define OPENTODOLIST_DATABASE_QUERIES_DELETETASK_H

#include "datamodel/task.h"
#include "database/queries/private/deleteobject.h"

namespace OpenTodoList {
namespace DataBase {
namespace Queries {

using namespace DataModel;
using namespace Private;

class DeleteTask : public DeleteObject<Task>
{
public:
  DeleteTask( Task *task );
  ~DeleteTask();
};

} // namespace Queries
} // namespace DataBase
} // namespace OpenTodoList

#endif // OPENTODOLIST_DATABASE_QUERIES_DELETETASK_H

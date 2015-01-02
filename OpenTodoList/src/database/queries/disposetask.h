#ifndef OPENTODOLIST_DATABASE_QUERIES_DISPOSETASK_H
#define OPENTODOLIST_DATABASE_QUERIES_DISPOSETASK_H

#include "datamodel/task.h"
#include "database/queries/private/disposeobject.h"

namespace OpenTodoList {
namespace DataBase {
namespace Queries {

using namespace Private;
using namespace DataModel;

class DisposeTask : public DisposeObject<Task>
{
public:
  DisposeTask( Task *task );
  ~DisposeTask();
};

} // namespace Queries
} // namespace DataBase
} // namespace OpenTodoList

#endif // OPENTODOLIST_DATABASE_QUERIES_DISPOSETASK_H

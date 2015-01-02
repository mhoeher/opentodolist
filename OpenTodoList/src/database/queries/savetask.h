#ifndef OPENTODOLIST_DATABASE_QUERIES_SAVETASK_H
#define OPENTODOLIST_DATABASE_QUERIES_SAVETASK_H

#include "datamodel/task.h"
#include "database/queries/private/saveobject.h"

namespace OpenTodoList {
namespace DataBase {
namespace Queries {

using namespace Private;
using namespace DataModel;

class SaveTask : public SaveObject<Task>
{
public:
  SaveTask( Task *task );
  ~SaveTask();
};

} // namespace Queries
} // namespace DataBase
} // namespace OpenTodoList

#endif // OPENTODOLIST_DATABASE_QUERIES_SAVETASK_H

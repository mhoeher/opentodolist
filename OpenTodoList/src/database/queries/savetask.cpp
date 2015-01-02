#include "savetask.h"

namespace OpenTodoList {
namespace DataBase {
namespace Queries {

SaveTask::SaveTask(Task *task) :
  SaveObject<Task>( task )
{

}

SaveTask::~SaveTask()
{

}

} // namespace Queries
} // namespace DataBase
} // namespace OpenTodoList


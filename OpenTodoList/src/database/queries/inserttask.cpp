#include "inserttask.h"

#include <QTextStream>

namespace OpenTodoList {
namespace DataBase {
namespace Queries {

InsertTask::InsertTask(Task *task, bool update) :
    InsertObject<Task>(
      task,
      "task",
      "taskMetaAttributeName",
      "taskMetaAttribute",
      { "uuid", "weight", "done", "title" },
      update,
      [this, task] { emit this->taskChanged( task->toVariant()); },
      "todo",
      "uuid" )
{
}

} // namespace Queries
} // namespace DataBase
} // namespace OpenTodoList

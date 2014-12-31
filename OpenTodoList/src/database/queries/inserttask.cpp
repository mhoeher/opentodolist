#include "inserttask.h"

#include <QTextStream>

namespace OpenTodoList {
namespace DataBase {
namespace Queries {

InsertTask::InsertTask(Task *task, bool update) :
    InsertObject<Task>(
      task,
      { "uuid", "weight", "done", "title" },
      update )
{
  connect( this, &InsertTask::queryFinished,
           [this,task] { emit this->taskChanged( task->toVariant() ); } );
}

} // namespace Queries
} // namespace DataBase
} // namespace OpenTodoList

#include "deletetask.h"

namespace OpenTodoList {
namespace DataBase {
namespace Queries {

DeleteTask::DeleteTask(Task *task) :
  DeleteObject( task )
{
  connect( this, &DeleteTask::queryFinished,
           [this,task] {
    emit this->taskDeleted( task->toVariant() );
  });
}

DeleteTask::~DeleteTask()
{

}

} // namespace Queries
} // namespace DataBase
} // namespace OpenTodoList


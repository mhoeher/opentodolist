#include "disposetask.h"

namespace OpenTodoList {
namespace DataBase {
namespace Queries {

DisposeTask::DisposeTask(Task *task) :
  DisposeObject<Task>( task )
{
  connect( this, &DisposeTask::queryFinished,
           [this,task] {
    task->setDisposed( true );
    emit this->taskChanged( task->toVariant() );
  });
}

DisposeTask::~DisposeTask()
{

}

} // namespace Queries
} // namespace DataBase
} // namespace OpenTodoList


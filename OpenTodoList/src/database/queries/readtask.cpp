#include "readtask.h"

#include <QTextStream>

namespace OpenTodoList {
namespace DataBase {
namespace Queries {

ReadTask::ReadTask() :
  ReadObject<Task>( { "uuid", "weight", "done", "title" } )
{
  connect( this, &ReadTask::queryFinished,
           [this] {
    for ( Task *task : this->objects() ) {
      emit this->readTask( task->toVariant() );
    }
  });
}

int ReadTask::todoId() const
{
  return parentId().toInt();
}

void ReadTask::setTodoId(int todoId)
{
  setParentId( todoId );
}


} // namespace Queries
} // namespace DataBase
} // namespace OpenTodoList

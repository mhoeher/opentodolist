#include "readtodo.h"

#include <QTextStream>

namespace OpenTodoList {
namespace DataBase {
namespace Queries {

ReadTodo::ReadTodo() :
    ReadObject<Todo>(
      { "uuid", "weight", "done", "priority", "dueDate", "title", "description" } ),
    m_todoListUuid( QUuid() ),
    m_minDueDate( QDateTime() ),
    m_maxDueDate( QDateTime() ),
    m_showDone( true )
{
  connect( this, &ReadTodo::queryFinished,
           [this] {
    for ( Todo* todo : this->objects() ) {
      emit this->readTodo( todo->toVariant() );
    }
  });
}

QList<Todo *> ReadTodo::todos() const
{
    return objects();
}

QUuid ReadTodo::todoListUuid() const
{
    return m_todoListUuid;
}

void ReadTodo::setTodoListUuid(const QUuid &todoListUuid)
{
    m_todoListUuid = todoListUuid;
}

QDateTime ReadTodo::minDueDate() const
{
    return m_minDueDate;
}

void ReadTodo::setMinDueDate(const QDateTime &minDueDate)
{
    m_minDueDate = minDueDate;
}

QDateTime ReadTodo::maxDueDate() const
{
    return m_maxDueDate;
}

void ReadTodo::setMaxDueDate(const QDateTime &maxDueDate)
{
    m_maxDueDate = maxDueDate;
}
bool ReadTodo::showDone() const
{
    return m_showDone;
}

void ReadTodo::setShowDone(bool showDone)
{
    m_showDone = showDone;
}

} // namespace Queries
} // namespace DataBase
} // namespace OpenTodoList

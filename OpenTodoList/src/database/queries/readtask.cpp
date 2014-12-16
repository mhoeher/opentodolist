#include "readtask.h"

#include <QTextStream>

namespace OpenTodoList {
namespace DataBase {
namespace Queries {

ReadTask::ReadTask() :
  StorageQuery(),
  m_tasks(),
  m_todoId(-1),
  m_currentTask( nullptr )
{
}

bool ReadTask::query(QString &query, QVariantMap &args)
{
  QTextStream stream( &query );
  QStringList conditions;

  stream << "SELECT "
         << " task.id AS id,"
         << " task.uuid AS uuid,"
         << " task.weight AS weight,"
         << " task.done AS done,"
         << " task.title AS title,"
         << " taskMetaAttribute.value AS metaAttributeValue,"
         << " taskMetaAttributeName.name AS metaAttributeName "
         << "FROM task "
         << "LEFT OUTER JOIN taskMetaAttribute "
         << " ON task.id = taskMetaAttribute.task "
         << "INNER JOIN taskMetaAttributeName "
         << " ON taskMetaAttributeName.id = taskMetaAttribute.attributeName ";
  if ( m_todoId >= 0 ) {
    conditions << "(todo = :todoId)";
    args.insert( "todoId", m_todoId );
  }
  if ( !conditions.isEmpty() ) {
    stream << " WHERE " << conditions.join( " AND " );
  }
  stream << ";";
  return true;
}

void ReadTask::recordAvailable(const QVariantMap &record)
{
  if ( m_currentTask && m_currentTask->id() != record.value( "id" ).toInt() ) {
    m_tasks << m_currentTask;
    emit readTask( m_currentTask->toVariant() );
    m_currentTask = nullptr;
  }
  if ( !m_currentTask ) {
    m_currentTask = new Task( this );
    m_currentTask->setId( record.value( "id" ).toInt() );
    m_currentTask->setUuid( record.value( "uuid" ).toUuid() );
    m_currentTask->setWeight( record.value( "weight" ).toDouble() );
    m_currentTask->setDone( record.value( "done" ).toBool() );
    m_currentTask->setTitle( record.value( "title" ).toString() );
  }
  if ( record.contains( "metaAttributeName") ) {
    QVariantMap attrs = m_currentTask->metaAttributes();
    attrs.insert( record.value( "metaAttributeName" ).toString(),
                  record.value( "metaAttributeValue" ) );
    m_currentTask->setMetaAttributes( attrs );
  }
}

void ReadTask::endRun()
{
  if ( m_currentTask != nullptr ) {
    m_tasks << m_currentTask;
    emit readTask( m_currentTask->toVariant() );
    m_currentTask = nullptr;
  }
}

QList<Task *> ReadTask::tasks() const
{
  return m_tasks;
}

int ReadTask::todoId() const
{
  return m_todoId;
}

void ReadTask::setTodoId(int todoId)
{
  m_todoId = todoId;
}


} // namespace Queries
} // namespace DataBase
} // namespace OpenTodoList

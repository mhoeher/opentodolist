#include "taskmodel.h"

#include "database/queries/readtask.h"

namespace OpenTodoList {
namespace Models {

TaskModel::TaskModel(QObject *parent) :
  QAbstractListModel(parent),
  m_database( nullptr ),
  m_tasks(),
  m_todo()
{
  connect( this, &TaskModel::databaseChanged, this, &TaskModel::refresh );
  connect( this, &TaskModel::todoChanged, this, &TaskModel::refresh );
}

Database *TaskModel::database() const
{
  return m_database;
}

void TaskModel::setDatabase(Database *database)
{
  if ( m_database != database ) {
    m_database = database;
    emit databaseChanged();
  }
}

Todo *TaskModel::todo() const
{
  return m_todo.data();
}

void TaskModel::setTodo(Todo *todo)
{
  if ( m_todo.data() != todo ) {
    if ( !m_todo.isNull() ) {
      disconnect( m_todo.data(), &Todo::idChanged, this, &TaskModel::refresh );
    }
    m_todo = todo;
    if ( !m_todo.isNull() ) {
      connect( m_todo.data(), &Todo::idChanged, this, &TaskModel::refresh );
    }
    emit todoChanged();
  }
}

int TaskModel::rowCount(const QModelIndex &parent) const
{
  if ( parent.isValid() ) {
    return 0;
  } else {
    return m_tasks.size();
  }
}

QVariant TaskModel::data(const QModelIndex &index, int role) const
{
  if ( index.isValid() && index.row() < m_tasks.size() ) {
    switch ( role ) {
    case Qt::DisplayRole:
      return QVariant::fromValue< QObject* >( m_tasks.at( index.row() ) );
    default:
      break;
    }
  }
  return QVariant();
}

void TaskModel::refresh()
{
  if ( m_database && ( m_todo.isNull() || m_todo->hasId() ) ) {
    Queries::ReadTask *query = new Queries::ReadTask();
    if ( !m_todo.isNull() ) {
      query->setTodoId( m_todo->id() );
    }
    connect( query, &Queries::ReadTask::readTask, this, &TaskModel::addTask );
    m_database->scheduleQuery( query );
  }
}

void TaskModel::addTask(const QVariant &task)
{
  Task *t = new Task( this );
  t->fromVariant( task );

  // check if we already have this one:
  for ( Task *existingTask : m_tasks ) {
    if ( existingTask->uuid() == t->uuid() ) {
      existingTask->fromVariant( task );
      delete t;
      return;
    }
  }

  // Append:
  beginInsertRows( QModelIndex(), m_tasks.size(), m_tasks.size() );
  m_tasks.append( t );
  endInsertRows();
}



} // namespace Models
} // namespace OpenTodoList

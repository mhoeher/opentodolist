/*
 *  OpenTodoList - A todo and task manager
 *  Copyright (C) 2014 - 2015 Martin Höher <martin@rpdev.net>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "taskmodel.h"

#include "datamodel/objectinfo.h"

#include "database/queries/readtask.h"
#include "database/databaseconnection.h"

namespace OpenTodoList {
namespace Models {

TaskModel::TaskModel(QObject *parent) :
  ObjectModel(ObjectInfo<Task>::classUuidProperty(), parent),
  m_todo()
{
  setTextProperty( "title" );
  connect( this, &TaskModel::databaseChanged, this, &TaskModel::refresh );
  connect( this, &TaskModel::todoChanged, this, &TaskModel::refresh );

  connect( this, &TaskModel::objectAdded, [this] (QObject *object) {
    Task *task = dynamic_cast< Task* >( object );
    if ( task ) {
      this->connect( task, &Task::changed, [task,this] {
        if ( this->database() ) {
          DatabaseConnection conn;
          conn.setDatabase( this->database() );
          conn.insertTask( task );
        }
      });
    }
  });
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

void TaskModel::moveTask(Task *task, TaskModel::MoveTaskMode mode, Task *target)
{
  if ( task && target ) {
    int taskIndex = objectList().indexOf( task );
    int targetIndex = objectList().indexOf( target );
    if ( taskIndex != targetIndex ) {
      int target2Index = -1;
      if ( mode == MoveTaskBefore ) {
        if ( targetIndex > 0 ) {
          target2Index = targetIndex - 1;
        } else {
          task->setWeight( target->weight() - qrand() % 1000 / 1000.0 );
          move( task, 0 );
        }
      } else {
        if ( targetIndex < objectList().size() - 1 ) {
          target2Index = targetIndex + 1;
        } else {
          task->setWeight( target->weight() + qrand() % 1000 / 1000.0 );
          move( task, objectList().size() );
        }
      }
      if ( target2Index >= 0 && target2Index != taskIndex ) {
        auto w1 = target->weight();
        auto w2 = qobject_cast<Task*>(objectList().at(target2Index) )->weight();
        task->setWeight( ( w1 + w2 ) / 2.0 );
        move( task, qMax( targetIndex, target2Index ) );
      }
    }
  }
}

void TaskModel::connectToDatabase()
{
  connect( database(), &Database::taskChanged, this, &TaskModel::addTask );
  connect( database(), &Database::taskDeleted, this, &TaskModel::removeTask );
}

void TaskModel::disconnectFromDatabase()
{
  connect( database(), &Database::taskChanged, this, &TaskModel::addTask );
  connect( database(), &Database::taskDeleted, this, &TaskModel::removeTask );
}

StorageQuery *TaskModel::createQuery() const
{
  Queries::ReadTask *query = new Queries::ReadTask();
  if ( !m_todo.isNull() ) {
    query->setTodoId( m_todo->id() );
  }
  connect( query, &Queries::ReadTask::readTask, this, &TaskModel::addTask );
  return query;
}

int TaskModel::compareObjects(QObject *left, QObject *right) const
{
  Task *leftTask = dynamic_cast< Task* >( left );
  Task *rightTask = dynamic_cast< Task* >( right );
  if ( leftTask && rightTask ) {
    // Show done tasks at end of list
    if ( leftTask->done() && !rightTask->done() ) {
      return 1;
    } else if ( !leftTask->done() && rightTask->done() ) {
      return -1;
    }
    // Default: Sort by name
    return leftTask->weight() < rightTask->weight() ?
          -1 : leftTask->weight() > rightTask->weight() ? 1 : 0;
  }
  return 0;
}

void TaskModel::addTask(const QVariant &task)
{
  addObject<Task>( task );
}

void TaskModel::removeTask(const QVariant &task)
{
  removeObject<Task>( task );
}



} // namespace Models
} // namespace OpenTodoList

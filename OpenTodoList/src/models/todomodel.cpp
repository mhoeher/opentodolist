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

#include "models/todomodel.h"

#include "datamodel/objectinfo.h"

#include "datamodel/todo.h"

#include "database/databaseconnection.h"
#include "database/queries/readtodo.h"

#include <QTimer>

namespace OpenTodoList {

namespace Models {

TodoModel::TodoModel(QObject *parent) :
    ObjectModel( ObjectInfo<Todo>::classUuidProperty(), parent),
    m_todoList( 0 ),
    m_filter( QString() ),
    m_showDone( false ),
    m_maxDueDate( QDateTime() ),
    m_minDueDate( QDateTime() ),
    m_sortMode( TodoModel::SortTodoByName ),
    m_backendSortMode( TodoModel::SortTodoByName ),
    m_limitOffset( -1 ),
    m_limitCount( -1 )
{
    setTextProperty("title");
    connect( this, &TodoModel::todoListChanged, this, &TodoModel::refresh );
    connect( this, &TodoModel::queryTypeChanged, this, &TodoModel::refresh );
    connect( this, &TodoModel::filterChanged, this, &TodoModel::refresh );
    connect( this, &TodoModel::showDoneChanged, this, &TodoModel::refresh );
    connect( this, &TodoModel::maxDueDateChanged, this, &TodoModel::refresh );
    connect( this, &TodoModel::minDueDateChanged, this, &TodoModel::refresh );

    connect( this, &TodoModel::objectAdded, [this] (QObject *object) {
      Todo *todo = dynamic_cast< Todo* >( object );
      if ( todo ) {
        this->connect( todo, &Todo::changed, [todo,this] {
          if ( this->database() ) {
            DatabaseConnection conn;
            conn.setDatabase( this->database() );
            conn.insertTodo( todo );
          }
        });
      }
    });
}

TodoModel::~TodoModel()
{
}

void TodoModel::connectToDatabase()
{
  connect( database(), &Database::todoChanged, this, &TodoModel::addTodo );
  connect( database(), &Database::todoDeleted, this, &TodoModel::removeTodo );
}

void TodoModel::disconnectFromDatabase()
{
  disconnect( database(), &Database::todoChanged, this, &TodoModel::addTodo );
  disconnect( database(), &Database::todoDeleted, this, &TodoModel::removeTodo );
}

StorageQuery *TodoModel::createQuery() const
{
  Queries::ReadTodo *query = new Queries::ReadTodo();
  if ( !m_todoList.isNull() ) {
    if ( m_todoList->hasId() ) {
      query->setParentId( m_todoList->id() );
    } else {
      query->setParentName( m_todoList->uuid() );
    }
  }
  query->setLimit( m_limitCount );
  query->setOffset( m_limitOffset );
  query->setMinDueDate( m_minDueDate );
  query->setMaxDueDate( m_maxDueDate );
  query->setShowDone( m_showDone );
  query->setFilter( m_filter );
  connect( query, &Queries::ReadTodo::readTodo, this, &TodoModel::addTodo, Qt::QueuedConnection );
  return query;
}

bool TodoModel::objectFilter(QObject *object) const
{
  Todo *todo = dynamic_cast< Todo* >( object );
  if ( todo ) {
    return ( !m_minDueDate.isValid() ||  ( todo->dueDate().isValid() && m_minDueDate <= todo->dueDate() ) ) &&
           ( !m_maxDueDate.isValid() || ( todo->dueDate().isValid() && todo->dueDate() <= m_maxDueDate ) ) &&
           ( m_showDone || !todo->done() ) &&
           ( m_filter.isEmpty() || todo->title().contains( m_filter, Qt::CaseInsensitive )
                                   || todo->description().contains( m_filter, Qt::CaseInsensitive ) );
  }
  return false;
}

int TodoModel::compareObjects(QObject *left, QObject *right) const
{
  Todo* leftTodo = dynamic_cast<Todo*>( left );
  Todo* rightTodo = dynamic_cast<Todo*>( right );
  if ( leftTodo && rightTodo ) {
      // always show open todos before done ones:
      if ( leftTodo->done() && !rightTodo->done() ) {
          return 1;
      }
      if ( rightTodo->done() && !leftTodo->done() ) {
          return -1;
      }

      // sort depending on mode
      switch ( m_sortMode ) {

      // "default" sorting ;) Applied everywhere
      case SortTodoByName:
          //return leftTodo->title().localeAwareCompare( rightTodo->title() ) <= 0;
          break;

      case SortTodoByPriority:
          if ( leftTodo->priority() != rightTodo->priority() ) {
              return rightTodo->priority() - leftTodo->priority();
          }
          break;

      case SortTodoByDueDate:
          if ( leftTodo->dueDate() != rightTodo->dueDate() ) {
              if ( !leftTodo->dueDate().isValid() ) {
                  return 1;
              }
              if ( !rightTodo->dueDate().isValid() ) {
                  return -1;
              }
              return leftTodo->dueDate() < rightTodo->dueDate() ? -1 : 1;
          }
          break;

      case SortTodoByWeight:
          return ( leftTodo->weight() < rightTodo->weight() );
      }

      // compare everything else by title
      return leftTodo->title().localeAwareCompare( rightTodo->title() );
  }
  return 0;
}

void TodoModel::addTodo(const QVariant &todo)
{
  addObject<Todo>(todo);
}

void TodoModel::removeTodo(const QVariant &todo)
{
  removeObject<Todo>(todo);
}

int TodoModel::limitCount() const
{
    return m_limitCount;
}

void TodoModel::setLimitCount(int limitCount)
{
    if ( m_limitCount != limitCount ) {
        m_limitCount = limitCount;
        emit limitCountChanged();
    }
}

int TodoModel::limitOffset() const
{
    return m_limitOffset;
}

void TodoModel::setLimitOffset(int limitOffset)
{
    if ( m_limitOffset != limitOffset ) {
        m_limitOffset = limitOffset;
        emit limitOffsetChanged();
    }
}

TodoModel::TodoSortMode TodoModel::backendSortMode() const
{
    return m_backendSortMode;
}

void TodoModel::setBackendSortMode(const TodoModel::TodoSortMode &backendSortMode)
{
    if ( m_backendSortMode != backendSortMode ) {
        m_backendSortMode = backendSortMode;
        emit backendSortModeChanged();
    }
}

QDateTime TodoModel::minDueDate() const
{
    return m_minDueDate;
}

void TodoModel::setMinDueDate(const QDateTime &minDueDate)
{
    if ( m_minDueDate != minDueDate ) {
        m_minDueDate = minDueDate;
        emit minDueDateChanged();
    }
}

TodoModel::TodoSortMode TodoModel::sortMode() const
{
    return m_sortMode;
}

void TodoModel::setSortMode(const TodoModel::TodoSortMode &sortMode)
{
    if ( m_sortMode != sortMode ) {
        m_sortMode = sortMode;
        sort();
        emit sortModeChanged();
    }
}

QDateTime TodoModel::maxDueDate() const
{
    return m_maxDueDate;
}

void TodoModel::setMaxDueDate(const QDateTime &maxDueDate)
{
    if ( m_maxDueDate != maxDueDate ) {
        m_maxDueDate = maxDueDate;
        emit maxDueDateChanged();
    }
}

bool TodoModel::showDone() const
{
    return m_showDone;
}

void TodoModel::setShowDone(bool showDone)
{
    if ( m_showDone != showDone ) {
        m_showDone = showDone;
        emit showDoneChanged();
    }
}

QString TodoModel::filter() const
{
    return m_filter;
}

void TodoModel::setFilter(const QString &filter)
{
    if ( m_filter != filter ) {
        m_filter = filter;
        emit filterChanged();
    }
}

DataModel::TodoList *TodoModel::todoList() const
{
    return m_todoList.data();
}

void TodoModel::setTodoList(DataModel::TodoList *todoList)
{
    if ( m_todoList != todoList ) {
        m_todoList = todoList;
        emit todoListChanged();
    }
}

} /* Models */

} /* OpenTodoList */

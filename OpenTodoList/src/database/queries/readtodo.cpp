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

#include "readtodo.h"

#include <QTextStream>

namespace OpenTodoList {
namespace DataBase {
namespace Queries {

ReadTodo::ReadTodo() :
    ReadObject<Todo>(
      { "uuid", "weight", "done", "priority", "dueDate", "title", "description" } ),
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

ReadTodo::ConditionList ReadTodo::generatedConditions() const
{
  ConditionList result;
  if ( m_minDueDate.isValid() ) {
    Condition c;
    c.condition = "(dueDate IS NOT NULL AND dueDate >= :readTodoMinDueDate)";
    c.arguments.insert( "readTodoMinDueDate", m_minDueDate );
    result.append( c );
  }
  if ( m_maxDueDate.isValid() ) {
    Condition c;
    c.condition = "(dueDate IS NOT NULL AND dueDate <= :readTodoMaxDueDate)";
    c.arguments.insert( "readTodoMaxDueDate", m_maxDueDate );
    result.append( c );
  }
  if ( !m_showDone ) {
    Condition c;
    c.condition = "(NOT done)";
    result.append( c );
  }
  if ( !m_filter.isEmpty() ) {
    Condition c;
    c.condition = "( title LIKE '%:readTodoFilterInTitle%' OR "
                    "description LIKE '%:readTodoFilterInDescription%' )";
    c.arguments.insert( "readTodoFilterInTitle", m_filter );
    c.arguments.insert( "readTodoFilterInDescription", m_filter );
  }
  return result;
}

QString ReadTodo::filter() const
{
  return m_filter;
}

void ReadTodo::setFilter(const QString &filter)
{
  m_filter = filter;
}

} // namespace Queries
} // namespace DataBase
} // namespace OpenTodoList

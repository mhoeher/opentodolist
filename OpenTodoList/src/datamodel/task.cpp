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

#include "task.h"

namespace OpenTodoList {

namespace DataModel {

Task::Task(QObject *parent) :
  QObject(parent),
  m_id( -1 ),
  m_hasId( false ),
  m_uuid(),
  m_title(),
  m_done(),
  m_weigth( 0.0 ),
  m_metaAttributes(),
  m_todoUuid(),
  m_dirty( 0 ),
  m_disposed( false ),
  m_loading( false )
{
  connect( this, &Task::titleChanged, this, &Task::emitChanged );
  connect( this, &Task::doneChanged, this, &Task::emitChanged );
  connect( this, &Task::weightChanged, this, &Task::emitChanged );
  connect( this, &Task::todoChanged, this, &Task::emitChanged );
}

int Task::id() const
{
  return m_id;
}

void Task::setId(int id)
{
  m_hasId = true;
  if ( m_id != id ) {
    m_id = id;
    emit idChanged();
  }
}

bool Task::hasId() const
{
  return m_hasId;
}

QUuid Task::uuid() const
{
  return m_uuid;
}

void Task::setUuid(const QUuid &uuid)
{
  m_uuid = uuid;
}

QString Task::title() const
{
  return m_title;
}

void Task::setTitle(const QString &title)
{
  if ( m_title != title ) {
    m_title = title;
    emit titleChanged();
  }
}

bool Task::done() const
{
  return m_done;
}

void Task::setDone(bool done)
{
  if ( m_done != done ) {
    m_done = done;
    emit doneChanged();
  }
}

double Task::weight() const
{
  return m_weigth;
}

void Task::setWeight(double weight)
{
  if ( m_weigth != weight ) {
    m_weigth = weight;
    emit weightChanged();
  }
}

QVariantMap Task::metaAttributes() const
{
  return m_metaAttributes;
}

void Task::setMetaAttributes(const QVariantMap &metaAttributes)
{
  m_metaAttributes = metaAttributes;
  emit metaAttributesChanged();
}

QUuid Task::todo() const
{
  return m_todoUuid;
}

void Task::setTodo(const QUuid &uuid)
{
  if ( m_todoUuid != uuid ) {
    m_todoUuid = uuid;
  }
}

QVariant Task::toVariant() const
{
  QVariantMap result;
  if ( m_hasId ) {
    result.insert( "id", m_id );
  }
  result.insert( "done", m_done );
  result.insert( "metaAttributes", m_metaAttributes );
  result.insert( "title", m_title );
  result.insert( "todoUuid", m_todoUuid );
  result.insert( "uuid", m_uuid );
  result.insert( "weight", m_weigth );
  result.insert( "dirty", m_dirty );
  result.insert( "disposed", m_disposed );

  return result;
}

void Task::fromVariant(const QVariant &task)
{
  m_loading = true;
  QVariantMap map = task.toMap();

  // Special handling ID: Onlz override on identity change
  if ( map.value( "uuid" ).toUuid() != m_uuid || !m_hasId ) {
    if ( map.contains( "id" ) ) {
      setId( map.value( "id" ).toInt() );
    }
  }

  setDone( map.value( "done", m_done ).toBool() );
  setMetaAttributes( map.value( "metaAttributes", m_metaAttributes ).toMap() );
  setTitle( map.value( "title", m_title ).toString() );
  setTodo( map.value( "todoUuid", m_todoUuid ).toUuid() );
  setUuid( map.value( "uuid", m_uuid ).toUuid() );
  setWeight( map.value( "weight", m_weigth ).toDouble() );
  setDirty( map.value( "dirty", m_dirty ).toInt() );
  setDisposed( map.value( "disposed", m_disposed ).toBool() );

  m_loading = false;
}
int Task::dirty() const
{
  return m_dirty;
}

void Task::setDirty(int dirty)
{
  m_dirty = dirty;
}
bool Task::disposed() const
{
  return m_disposed;
}

void Task::setDisposed(bool disposed)
{
  m_disposed = disposed;
}

void Task::toggle()
{
  setDone( !done() );
}

void Task::emitChanged()
{
  if ( !m_loading ) {
    emit changed();
  }
}



} // namespace DataModel
} // namespace OpenTodoList

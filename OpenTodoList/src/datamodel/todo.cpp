/*
 *  OpenTodoList - A todo and task manager
 *  Copyright (C) 2013, 2014  Martin Höher <martin@rpdev.net>
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

#include "todo.h"

#include <QDebug>

namespace OpenTodoList {

namespace DataModel {

/**
   @brief Creates a new Todo
 */
Todo::Todo(QObject *parent) :
  QObject( parent ),
  m_id( -1 ),
  m_hasId( false ),
  m_uuid( QUuid() ),
  m_weight(0.0),
  m_done( false ),
  m_priority( -1 ),
  m_todoListUuid( QUuid() ),
  m_dueDate( QDateTime() ),
  m_title( QString() ),
  m_description( QString() ),
  m_metaAttributes( QVariantMap() ),
  m_dirty( 0 ),
  m_disposed( false ),
  m_loading( false )
{
  setupTodo();
}

/**
   @brief Destructor
 */
Todo::~Todo()
{
}

const QUuid &Todo::uuid() const
{
  return m_uuid;
}

void Todo::setUuid(const QUuid &uuid)
{
  if ( m_uuid != uuid ) {
    m_uuid = uuid;
    emit uuidChanged();
  }
}

double Todo::weight() const
{
  return m_weight;
}

void Todo::setWeight(double weight)
{
  if ( m_weight != weight ) {
    m_weight = weight;
    emit weightChanged();
  }
}

bool Todo::done() const
{
  return m_done;
}

void Todo::setDone(bool done)
{
  if ( m_done != done ) {
    m_done = done;
    emit doneChanged();
  }
}

int Todo::priority() const
{
  return qBound( -1, m_priority, 10 );
}

void Todo::setPriority(int priority)
{
  if ( m_priority != priority ) {
    m_priority = priority;
    emit priorityChanged();
  }
}

const QUuid &Todo::todoList() const
{
  return m_todoListUuid;
}

void Todo::setTodoList(const QUuid &uuid)
{
  if ( m_todoListUuid != uuid ) {
    m_todoListUuid = uuid;
    emit todoListUuidChanged();
  }
}

const QDateTime &Todo::dueDate() const
{
  return m_dueDate;
}

void Todo::setDueDate(const QDateTime &dueOn)
{
  if ( m_dueDate != dueOn ) {
    m_dueDate = dueOn;
    emit dueDateChanged();
  }
}

const QString &Todo::title() const
{
  return m_title;
}

void Todo::setTitle(const QString &title)
{
  if ( m_title != title ) {
    m_title = title;
    emit titleChanged();
  }
}

const QString &Todo::description() const
{
  return m_description;
}

void Todo::setDescription(const QString &description)
{
  if ( m_description != description ) {
    m_description = description;
    emit descriptionChanged();
  }
}

QVariantMap Todo::metaAttributes() const
{
  return m_metaAttributes;
}

void Todo::setMetaAttributes(const QVariantMap &metaAttributes)
{
  m_metaAttributes = metaAttributes;
  emit metaAttributesChanged();
}

/**
   @brief Creates a representation of the todo as QVariant

   This converts the Todo to a QVariant. Use Todo::fromVariant() to reverse the operation.
 */
QVariant Todo::toVariant() const
{
  QVariantMap result;
  result.insert( "description", m_description );
  result.insert( "done", m_done );
  result.insert( "dueDate", m_dueDate );
  if ( m_hasId ) {
    result.insert( "id", m_id );
  }
  result.insert( "metaAttributes", m_metaAttributes );
  result.insert( "priority", m_priority );
  result.insert( "title", m_title );
  result.insert( "todoListUuid", m_todoListUuid );
  result.insert( "uuid", m_uuid );
  result.insert( "weight", m_weight );
  result.insert( "dirty", m_dirty );
  result.insert( "disposed", m_disposed );
  return result;
}

/**
   @brief Restore a Todo from a QVariant

   Restores the @p todo from a QVariant representation.

   @sa toVariant()
 */
void Todo::fromVariant(const QVariant &todo)
{
  m_loading = true;
  QVariantMap map = todo.toMap();

  // special handling for ID: Only override on identity change
  if ( map.value( "uuid" ).toUuid() != m_uuid || !m_hasId ) {
    if ( map.contains( "id" ) ) {
      setId( map.value( "id" ).toInt() );
    }
  }

  // restore properties
  setDescription( map.value( "description", m_description ).toString() );
  setDone( map.value( "done", m_done ).toBool() );
  setDueDate( map.value( "dueDate", m_dueDate ).toDateTime() );
  setMetaAttributes( map.value( "metaAttributes", m_metaAttributes ).toMap() );
  setPriority( map.value( "priority", m_priority ).toInt() );
  setTitle( map.value( "title", m_title ).toString() );
  setTodoList( map.value( "todoListUuid", m_todoListUuid ).toUuid() );
  setUuid( map.value( "uuid", m_uuid ).toUuid() );
  setWeight( map.value( "weight", m_weight ).toDouble() );
  setDirty( map.value( "dirty", m_dirty ).toInt() );
  setDisposed( map.value( "disposed", m_disposed ).toBool() );

  m_loading = false;
}

/**
   @brief Toggles the state of the todo between "done" and "not done"

   @sa setProgress
 */
void Todo::toggle()
{
  setDone( !done() );
}

void Todo::setupTodo()
{
  connect( this, &Todo::weightChanged, this, &Todo::emitChanged );
  connect( this, &Todo::doneChanged, this, &Todo::emitChanged );
  connect( this, &Todo::priorityChanged, this, &Todo::emitChanged );
  connect( this, &Todo::dueDateChanged, this, &Todo::emitChanged );
  connect( this, &Todo::titleChanged, this, &Todo::emitChanged );
  connect( this, &Todo::descriptionChanged, this, &Todo::emitChanged );
}

void Todo::emitChanged()
{
  if ( !m_loading ) {
    emit changed();
  }
}

bool Todo::disposed() const
{
  return m_disposed;
}

void Todo::setDisposed(bool disposed)
{
  m_disposed = disposed;
}

int Todo::dirty() const
{
  return m_dirty;
}

void Todo::setDirty(int dirty)
{
  m_dirty = dirty;
}


bool Todo::hasId() const
{
  return m_hasId;
}

int Todo::id() const
{
  return m_id;
}

void Todo::setId(int id)
{
  m_hasId = true;
  if ( m_id != id ) {
    m_id = id;
    emit idChanged();
  }
}

/**
   @brief Returns whether the todo is valid

   This returns true of the todo has a valid UUID.
 */
bool OpenTodoList::DataModel::Todo::isValid() {
  return !m_uuid.isNull();
}

} /* DataModel */

} /* OpenTodoList */

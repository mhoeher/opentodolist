/*
 *  OpenTodoList - A todo and task manager
 *  Copyright (C) 2013 - 2015  Martin Höher <martin@rpdev.net>
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

#include "datamodel/todolist.h"

namespace OpenTodoList {

namespace DataModel {

/**
   @brief Creates a new TodoList
 */
TodoList::TodoList(QObject *parent) :
  QObject( parent ),
  m_id( -1 ),
  m_hasId( false ),
  m_uuid( QUuid() ),
  m_name( QString() ),
  m_metaAttributes(),
  m_accountUuid( QUuid() ),
  m_dirty( 0 ),
  m_disposed( false ),
  m_loading( false )

{
  connect( this, &TodoList::nameChanged, this, &TodoList::emitChanged );
}

/**
   @brief Destructor
 */
TodoList::~TodoList()
{
}

const QUuid &TodoList::uuid() const
{
  return m_uuid;
}

void TodoList::setUuid(const QUuid &uuid)
{
  if ( m_uuid != uuid ) {
    m_uuid = uuid;
    emit uuidChanged();
  }
}

const QString &TodoList::name() const
{
  return m_name;
}

void TodoList::setName(const QString &name)
{
  if ( m_name != name ) {
    m_name = name;
    emit nameChanged();
  }
}

QVariantMap TodoList::metaAttributes() const
{
  return m_metaAttributes;
}

void TodoList::setMetaAttributes(const QVariantMap &metaAttributes)
{
  m_metaAttributes = metaAttributes;
  emit metaAttributesChanged();
}

QUuid TodoList::account() const
{
  return m_accountUuid;
}

void TodoList::setAccount(const QUuid &uuid)
{
  if ( m_accountUuid != uuid ) {
    m_accountUuid = uuid;
    emit accountUuidChanged();
  }
}

/**
   @brief The ID of the todo list in the database
   @sa hasId()
 */
int TodoList::id() const
{
  return m_id;
}

void TodoList::setId(int id)
{
  m_hasId = true;
  if ( m_id != id ) {
    m_id = id;
    emit idChanged();
  }
}

bool TodoList::hasId() const
{
  return m_hasId;
}

/**
   @brief Saves the todo list into a QVariant

   Use this to convert the todo list into a QVariant representation.

   @sa fromVariant()
 */
QVariant TodoList::toVariant() const
{
  QVariantMap result;
  result.insert( "accountUuid", m_accountUuid );
  if ( m_hasId ) {
    result.insert( "id", m_id );
  }
  result.insert( "metaAttributes", m_metaAttributes );
  result.insert( "name", m_name );
  result.insert( "uuid", m_uuid );
  result.insert( "dirty", m_dirty );
  result.insert( "disposed", m_disposed );
  return result;
}

/**
   @brief Restore a todo list from a QVariant representation
   @sa toVariant()
 */
void TodoList::fromVariant(const QVariant &todolist)
{
  m_loading = true;
  QVariantMap map = todolist.toMap();

  // Special handling for id: Only update if identify changes
  if ( m_uuid != map.value( "uuid", QUuid() ).toUuid() || !m_hasId ) {
    m_hasId = map.contains( "id" );
    if ( m_hasId ) {
      setId( map.value( "id", m_id ).toInt() );
    }
  }

  setAccount( map.value( "accountUuid", m_accountUuid ).toUuid() );
  setMetaAttributes( map.value( "metaAttributes", m_metaAttributes ).toMap() );
  setName( map.value( "name", m_name ).toString() );
  setUuid( map.value( "uuid", m_uuid ).toUuid() );
  setDirty( map.value( "dirty", m_dirty ).toInt() );
  setDisposed( map.value( "disposed", m_disposed ).toBool() );
  m_loading = false;
}
int TodoList::dirty() const
{
  return m_dirty;
}

void TodoList::setDirty(int dirty)
{
  m_dirty = dirty;
}
bool TodoList::disposed() const
{
  return m_disposed;
}

void TodoList::setDisposed(bool disposed)
{
  m_disposed = disposed;
}

void TodoList::emitChanged()
{
  if ( !m_loading ) {
    emit changed();
  }
}



} /* DataModel */

} /* OpenTodoList */

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

#include "backend.h"

#include "QVariant"
#include "QVariantMap"

namespace OpenTodoList {
namespace DataModel {

Backend::Backend(QObject *parent) :
  QObject(parent),
  m_hasId( false ),
  m_id( -1 ),
  m_name(),
  m_title(),
  m_description(),
  m_loading( false )
{
  connect( this, &Backend::idChanged, this, &Backend::emitChanged );
  connect( this, &Backend::nameChanged, this, &Backend::emitChanged );
  connect( this, &Backend::descriptionChanged, this, &Backend::emitChanged );
  connect( this, &Backend::titleChanged, this, &Backend::emitChanged );
}

bool Backend::hasId() const
{
  return m_hasId;
}

int Backend::id() const
{
  return m_id;
}

void Backend::setId(int id)
{
  m_hasId = true;
  if ( m_id != id ) {
    m_id = id;
    emit idChanged();
  }
}

QString Backend::name() const
{
  return m_name;
}

void Backend::setName(const QString &name)
{
  if ( m_name != name ) {
    m_name = name;
    emit nameChanged();
  }
}

QString Backend::title() const
{
  return m_title;
}

void Backend::setTitle(const QString &title)
{
  if ( m_title != title ) {
    m_title = title;
    emit titleChanged();
  }
}

QString Backend::description() const
{
  return m_description;
}

void Backend::setDescription(const QString &description)
{
  if ( m_description != description ) {
    m_description = description;
    emit descriptionChanged();
  }
}

/**
   @brief Persists the Backend object to a QVariant

   This returns a representation of the Backend object as a QVariant. Use Backend::fromVariant
   to create a Backend object from such a representation.
 */
QVariant Backend::toVariant() const
{
  QVariantMap result;
  result.insert( "description", m_description );
  if ( m_hasId ) {
    result.insert( "id", m_id );
  }
  result.insert( "name", m_name );
  result.insert( "title", m_title );
  return result;
}

/**
   @brief Restores a @p backend from persisted storage

   Use this to restore a Backend object from a representation as QVariant produced by the
   Backend::toVariant() method.
 */
void Backend::fromVariant(const QVariant &backend)
{
  m_loading = true;
  QVariantMap map = backend.toMap();

  // Special handling for id: Only update if identify changes
  if ( m_name != map.value( "name", QString() ).toString() || !m_hasId ) {
    m_hasId = map.contains( "id" );
    if ( m_hasId ) {
      setId( map.value( "id", m_id ).toInt() );
    }
  }

  setDescription( map.value( "description", m_description ).toString() );
  m_hasId = map.contains( "id" );
  if ( m_hasId ) {
    setId( map.value( "id", m_id ).toInt() );
  }
  setName( map.value( "name", m_name ).toString() );
  setTitle( map.value( "title", m_title ).toString() );

  m_loading = false;
}

void Backend::emitChanged()
{
  if ( !m_loading ) {
    emit changed();
  }
}

} // namespace DataModel
} // namespace OpenTodoList

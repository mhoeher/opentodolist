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

#include "readbackend.h"

namespace OpenTodoList {
namespace DataBase {
namespace Queries {

/**
   @brief Constructor

   Creates a new query. If @p parentizeBackends is true, the created backend
   objects will be made children of the query (and hence be deleted as soon as the
   query gets destroyed). Otherwise, the created objects are created without a parent.

   @note If you set parentizeBackends to false, either use backends() or connect to the
         readBackend() signal to collect the backends read from the database. Otherwise, the
         created objects will remain in memory.
 */
ReadBackend::ReadBackend() :
  StorageQuery(),
  m_backends(),
  m_currentBackend(nullptr)
{
}

/**
   @brief The list of backends read from the database
 */
QList<DataModel::Backend *> ReadBackend::backends() const
{
  return m_backends;
}

bool ReadBackend::query(QString &query, QVariantMap &args, int &options )
{
  Q_UNUSED( args );
  Q_UNUSED( options );
  query = "SELECT backend.id AS id, "
                 "backend.name AS name, "
                 "backend.title AS title, "
                 "backend.description AS description, "
                 "backendCapability.capability AS capability "
          "FROM backend LEFT OUTER JOIN backendCapability "
          "ON backend.id = backendCapability.backend;";
  return true;
}

void ReadBackend::recordAvailable(const QVariantMap &record)
{
  if ( m_currentBackend && m_currentBackend->id() != record.value( "id" ).toInt() ) {
    m_backends << m_currentBackend;
    emit readBackend( m_currentBackend->toVariant() );
    m_currentBackend = nullptr;
  }
  if ( !m_currentBackend ) {
    m_currentBackend = new DataModel::Backend( this );
    m_currentBackend->setId( record.value( "id", -1 ).toInt() );
    m_currentBackend->setName( record.value( "name", QString() ).toString() );
    m_currentBackend->setTitle( record.value( "title", QString() ).toString() );
    m_currentBackend->setDescription( record.value( "description", QString() ).toString() );
  }
  if ( record.contains( "capability" ) ) {
    QSet<DataModel::Backend::Capabilities> caps = m_currentBackend->capabilities();
    caps.insert( static_cast< DataModel::Backend::Capabilities>(
                   record.value( "capability" ).toInt() ) );
    m_currentBackend->setCapabilities( caps );
  }
}

void ReadBackend::endRun()
{
  if ( m_currentBackend ) {
    m_backends << m_currentBackend;
    emit readBackend( m_currentBackend->toVariant() );
    m_currentBackend = nullptr;
  }
}

} // namespace Queries
} // namespace DataBase
} // namespace OpenTodoList

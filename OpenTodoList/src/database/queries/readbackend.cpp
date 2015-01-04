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
  StorageQuery()
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
  query = "SELECT id, name, title, description FROM backend;";
  return true;
}

void ReadBackend::recordAvailable(const QVariantMap &record)
{
  DataModel::Backend *backend = new DataModel::Backend( this );
  backend->setId( record.value( "id", -1 ).toInt() );
  backend->setName( record.value( "name", QString() ).toString() );
  backend->setTitle( record.value( "title", QString() ).toString() );
  backend->setDescription( record.value( "description", QString() ).toString() );
  emit readBackend( backend->toVariant() );
  m_backends << backend;
}

} // namespace Queries
} // namespace DataBase
} // namespace OpenTodoList

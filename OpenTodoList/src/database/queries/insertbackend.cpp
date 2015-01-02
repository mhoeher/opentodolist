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

#include "insertbackend.h"

namespace OpenTodoList {
namespace DataBase {
namespace Queries {

/**
   @brief Constructor

   This will create a new InsertBackend query. The query object can be used to
   insert the @p backend into the database if it does not exist. Otherwise, it will
   update an existing instance (using the Backend::name() to determine whether the
   object already exists in the database).

   @note The query takes over ownership of the backend.
 */
InsertBackend::InsertBackend( Backend *backend ) :
    OpenTodoList::DataBase::StorageQuery(),
    m_backend( backend )
{
    Q_ASSERT( backend != nullptr );
}

/**
   @brief Destructor
 */
InsertBackend::~InsertBackend()
{
    delete m_backend;
}

bool InsertBackend::query(QString &query, QVariantMap &args)
{
    query = "INSERT OR REPLACE INTO backend ( id, name, title, description ) "
            "VALUES ( (SELECT id FROM backend WHERE name = :searchName ), :name, :title, :description );";
    args.insert( "searchName", m_backend->name() );
    args.insert( "name", m_backend->name() );
    args.insert( "title", m_backend->title() );
    args.insert( "description", m_backend->description() );
    return true;
}

void InsertBackend::newIdAvailable(const QVariant &id)
{
    if ( id.isValid() ) {
        m_backend->setId( id.toInt() );
    }
}

void InsertBackend::endRun()
{
    emit backendChanged( m_backend->toVariant() );
}

} // namespace Queries
} // namespace DataBase
} // namespace OpenTodoList

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

#include "backendmodel.h"

#include "datamodel/objectinfo.h"

#include "database/queries/readbackend.h"

namespace OpenTodoList {
namespace Models {

BackendModel::BackendModel(QObject *parent) :
    ObjectModel(ObjectInfo<Backend>::classUuidProperty(), parent)
{
  connect( this, &BackendModel::databaseChanged, this, &BackendModel::refresh );
}

void BackendModel::connectToDatabase()
{
  connect( database(), &Database::backendChanged, this, &BackendModel::addBackend );
}

void BackendModel::disconnectFromDatabase()
{
  disconnect( database(), &Database::backendChanged, this, &BackendModel::addBackend );
}

StorageQuery *BackendModel::createQuery() const
{
  Queries::ReadBackend *query = new Queries::ReadBackend();
  connect( query, &Queries::ReadBackend::readBackend,
           this, &BackendModel::addBackend, Qt::QueuedConnection );
  return query;
}

void BackendModel::addBackend(const QVariant &backend)
{
  addObject<Backend>( backend );
}


} // namespace Models
} // namespace OpenTodoList

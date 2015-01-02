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

#ifndef DISPOSEOBJECT_H
#define DISPOSEOBJECT_H

#include "database/storagequery.h"
#include "datamodel/objectinfo.h"

#include <QTextStream>

namespace OpenTodoList {
namespace DataBase {
namespace Queries {
namespace Private {

using namespace DataModel;

/**
  @brief Generic query to mark objects as disposed

  This query can be used to mark objects as disposed. Upon execution, the disposed flag
  of the query will be set to true (1) to mark the object as being disposed.
 */
template<typename T>
class DisposeObject : public StorageQuery
{
public:
  explicit DisposeObject( T *object );

  // StorageQuery interface
  bool query(QString &query, QVariantMap &args) override;

private:

  T *m_object;

};

template<typename T>
DisposeObject<T>::DisposeObject(T *object) :
  StorageQuery(),
  m_object( object )
{
  Q_ASSERT( m_object != nullptr );
  Q_ASSERT( m_object->hasId() );
}

template<typename T>
bool DisposeObject<T>::query(QString &query, QVariantMap &args)
{
  QTextStream stream( &query );
  stream << "UPDATE OR FAIL " << ObjectInfo<T>::classNameLowerFirst()
         << " SET disposed = 1 WHERE id = :searchId;";
  args.insert( "searchId", m_object->id() );
  return true;
}

} // namespace Private
} // namespace Queries
} // namespace DataBase
} // namespace OpenTodoList
#endif // DISPOSEOBJECT_H


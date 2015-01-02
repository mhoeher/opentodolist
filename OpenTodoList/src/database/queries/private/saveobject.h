/*
 *  OpenTodoList - A todo and task manager
 *  Copyright (C) 2014 - 2015  Martin Höher <martin@rpdev.net>
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

#ifndef SAVEOBJECT_H
#define SAVEOBJECT_H

#include "database/storagequery.h"

#include "datamodel/objectinfo.h"

#include <QTextStream>

namespace OpenTodoList {
namespace DataBase {
namespace Queries {
namespace Private {

using namespace DataModel;

/**
  @brief Generic query to mark objects as saved

  This query can be used to mark an object as saved. Execution of this query will cause the
  dirty flag in the database to be decreased by the dirty flag given in the object passed to this
  query (or set to 0 in case this would yield a negative number).
 */
template<typename T>
class SaveObject : public StorageQuery
{
public:

  explicit SaveObject( T *object );

  // StorageQuery interface
  bool query(QString &query, QVariantMap &args);

private:

  T *m_object;

};

template<typename T>
SaveObject<T>::SaveObject(T *object) :
  StorageQuery(),
  m_object( object )
{
  Q_ASSERT( m_object != nullptr );
  Q_ASSERT( m_object->hasId() );
}

template<typename T>
bool SaveObject<T>::query(QString &query, QVariantMap &args)
{
  QString baseName = ObjectInfo<T>::classNameLowerFirst();

  QTextStream stream( &query );
  stream << "UPDATE OR FAIL " << baseName
         << " SET dirty = MAX( 0, dirty - :decreaseValue )"
         << " WHERE id = :searchId;";
  args.insert( "decreaseValue", m_object->dirty() );
  args.insert( "searchId", m_object->id() );
  return true;
}

} // namespace Private
} // namespace Queries
} // namespace DataBase
} // OpenTodoList

#endif // SAVEOBJECT_H


#ifndef DELETEOBJECT_H
#define DELETEOBJECT_H

#include "database/storagequery.h"

#include "datamodel/objectinfo.h"

#include <QTextStream>

namespace OpenTodoList {
namespace DataBase {
namespace Queries {
namespace Private {

using namespace DataModel;

/**
  @brief Generic query to delete an object from the database

  This is a generic storage query that can be used to delete objects from the database.
  Note that this relies on the database to remove any "dependent" rows on its own using
  triggers.
 */
template<typename T>
class DeleteObject : public StorageQuery
{
public:

  explicit DeleteObject( T *object );

  // StorageQuery interface
  bool query(QString &query, QVariantMap &args) override;

private:

  T *m_object;

};

template<typename T>
DeleteObject<T>::DeleteObject(T *object) :
  StorageQuery(),
  m_object( object )
{
  Q_ASSERT( m_object != nullptr );
}

template<typename T>
bool DeleteObject<T>::query(QString &query, QVariantMap &args)
{
  QTextStream stream( &query );
  stream << "DELETE FROM " << ObjectInfo<T>::classNameLowerFirst() << " WHERE ";
  if ( m_object->hasId() ) {
    stream << " id = :searchId;";
    args.insert( "searchId", m_object->id() );
  } else {
    stream << " uuid = :searchUuid;";
    args.insert( "searchUuid", m_object->uuid() );
  }
  return true;
}

}
}
}
}
#endif // DELETEOBJECT_H


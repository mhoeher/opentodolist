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


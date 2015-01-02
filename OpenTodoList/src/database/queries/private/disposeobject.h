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
         << " SET disposed = 1 WHERE id = :seearchId;";
  args.insert( "searchId", m_object->id() );
  return true;
}

} // namespace Private
} // namespace Queries
} // namespace DataBase
} // namespace OpenTodoList
#endif // DISPOSEOBJECT_H


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
ReadBackend::ReadBackend(bool parentizeBackends) :
    StorageQuery(),
    m_parentizeBackends( parentizeBackends )
{
}

/**
   @brief The list of backends read from the database
 */
QList<DataModel::Backend *> ReadBackend::backends() const
{
    return m_backends;
}

bool ReadBackend::query(QString &query, QVariantMap &args)
{
    Q_UNUSED( args );
    query = "SELECT id, name, title, description FROM backend;";
    return true;
}

void ReadBackend::recordAvailable(const QVariantMap &record)
{
    DataModel::Backend *backend = new DataModel::Backend( m_parentizeBackends ? this : nullptr );
    backend->setId( record.value( "id", -1 ).toInt() );
    backend->setName( record.value( "name", QString() ).toString() );
    backend->setTitle( record.value( "title", QString() ).toString() );
    backend->setDescription( record.value( "description", QString() ).toString() );
    emit readBackend( backend );
    m_backends << backend;
}

} // namespace Queries
} // namespace DataBase
} // namespace OpenTodoList

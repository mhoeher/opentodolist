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

} // namespace Queries
} // namespace DataBase
} // namespace OpenTodoList

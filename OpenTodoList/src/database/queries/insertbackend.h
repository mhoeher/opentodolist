#ifndef OPENTODOLIST_DATABASE_QUERIES_INSERTBACKEND_H
#define OPENTODOLIST_DATABASE_QUERIES_INSERTBACKEND_H

#include "datamodel/backend.h"

#include "database/storagequery.h"

namespace OpenTodoList {
namespace DataBase {
namespace Queries {

using namespace DataModel;

/**
   @brief A query to insert or update a Backend into the database

   This class implements a query which can be used to insert or update
   Backend objects into the database.
 */
class InsertBackend : public OpenTodoList::DataBase::StorageQuery
{
    Q_OBJECT
public:
    explicit InsertBackend( Backend *backend );
    virtual ~InsertBackend();

    // StorageQuery interface
    bool query(QString &query, QVariantMap &args) override;

signals:

public slots:

private:
    Backend *m_backend;

};

} // namespace Queries
} // namespace DataBase
} // namespace OpenTodoList

#endif // OPENTODOLIST_DATABASE_QUERIES_INSERTBACKEND_H

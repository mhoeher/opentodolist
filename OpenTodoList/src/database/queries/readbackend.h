#ifndef OPENTODOLIST_DATABASE_QUERIES_READBACKEND_H
#define OPENTODOLIST_DATABASE_QUERIES_READBACKEND_H

#include "datamodel/backend.h"

#include "database/storagequery.h"

#include <QList>

namespace OpenTodoList {
namespace DataBase {
namespace Queries {

/**
   @brief A query for reading DataModel::Backend objects from the Database

   This class implements a query to get DataModel::Backend objects from the Database.
 */
class ReadBackend : public StorageQuery
{
    Q_OBJECT
public:
    explicit ReadBackend( bool parentizeBackends = true );

    QList<DataModel::Backend*> backends() const;

    // StorageQuery interface
    bool query(QString &query, QVariantMap &args);
    void recordAvailable(const QVariantMap &record);

signals:

    void readBackend( OpenTodoList::DataModel::Backend* backend );

public slots:

private:
    QList< DataModel::Backend* >    m_backends;
    bool                            m_parentizeBackends;
};

} // namespace Queries
} // namespace DataBase
} // namespace OpenTodoList

#endif // OPENTODOLIST_DATABASE_QUERIES_READBACKEND_H

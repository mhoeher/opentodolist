#ifndef OPENTODOLIST_DATABASE_QUERIES_INSERTACCOUNT_H
#define OPENTODOLIST_DATABASE_QUERIES_INSERTACCOUNT_H

#include "datamodel/account.h"

#include "database/storagequery.h"
#include "database/queries/private/insertobject.h"

#include <QList>

namespace OpenTodoList {
namespace DataBase {
namespace Queries {

using namespace Private;
using namespace DataModel;

/**
   @brief Inserts a single account into the database

   This query can be used to insert a single DataModel::Account object into the Database.
 */
class InsertAccount : public InsertObject<Account>
{
    Q_OBJECT
public:

    explicit InsertAccount( Account *account, bool update );
    virtual ~InsertAccount();

signals:

public slots:

private:

};

} // namespace Queries
} // namespace DataBase
} // namespace OpenTodoList

#endif // OPENTODOLIST_DATABASE_QUERIES_INSERTACCOUNT_H

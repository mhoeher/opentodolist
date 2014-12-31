#ifndef OPENTODOLIST_DATABASE_QUERIES_READACCOUNT_H
#define OPENTODOLIST_DATABASE_QUERIES_READACCOUNT_H

#include "datamodel/account.h"

#include "database/storagequery.h"
#include "database/queries/private/readobject.h"

#include <QList>

namespace OpenTodoList {
namespace DataBase {
namespace Queries {

using namespace Private;
using namespace DataModel;

class ReadAccount : public ReadObject<Account>
{
    Q_OBJECT
public:
    explicit ReadAccount();

    QList< DataModel::Account* > accounts() const;

signals:

    /**
       @brief An account has been read from the database

       This signal is emitted for every @p account read from the database. Use
       Account::fromVariant() to convert the variant to an Account object.
     */
    void readAccount( const QVariant &account );

public slots:

private:

};

} // namespace Queries
} // namespace DataBase
} // namespace OpenTodoList

#endif // OPENTODOLIST_DATABASE_QUERIES_READACCOUNT_H

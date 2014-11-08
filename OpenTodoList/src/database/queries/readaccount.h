#ifndef OPENTODOLIST_DATABASE_QUERIES_READACCOUNT_H
#define OPENTODOLIST_DATABASE_QUERIES_READACCOUNT_H

#include "datamodel/account.h"

#include "database/storagequery.h"

#include <QList>

namespace OpenTodoList {
namespace DataBase {
namespace Queries {

class ReadAccount : public StorageQuery
{
    Q_OBJECT
public:
    explicit ReadAccount();

    QList< DataModel::Account* > accounts() const;

    // StorageQuery interface
    bool query(QString &query, QVariantMap &args) override;
    void recordAvailable(const QVariantMap &record) override;
    void endRun();

signals:

    /**
       @brief An account has been read from the database

       This signal is emitted for every @p account read from the database. Use
       Account::fromVariant() to convert the variant to an Account object.
     */
    void readAccount( const QVariant &account );

public slots:

private:

    QList< DataModel::Account* >    m_accounts;
    DataModel::Account*             m_currentAccount;

};

} // namespace Queries
} // namespace DataBase
} // namespace OpenTodoList

#endif // OPENTODOLIST_DATABASE_QUERIES_READACCOUNT_H

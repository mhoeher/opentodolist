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
    explicit ReadAccount( bool parentizeAccounts );

    QList< DataModel::Account* > accounts() const;

    // StorageQuery interface
    bool query(QString &query, QVariantMap &args) override;
    void recordAvailable(const QVariantMap &record) override;
    void endRun();

signals:

    void readAccount( DataModel::Account *account );

public slots:

private:

    QList< DataModel::Account* >    m_accounts;
    DataModel::Account*             m_currentAccount;

    bool                            m_parentizeAccounts;

};

} // namespace Queries
} // namespace DataBase
} // namespace OpenTodoList

#endif // OPENTODOLIST_DATABASE_QUERIES_READACCOUNT_H

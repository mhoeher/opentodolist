#ifndef OPENTODOLIST_DATABASE_QUERIES_INSERTACCOUNT_H
#define OPENTODOLIST_DATABASE_QUERIES_INSERTACCOUNT_H

#include "datamodel/account.h"

#include "database/storagequery.h"

#include <QList>

namespace OpenTodoList {
namespace DataBase {
namespace Queries {

/**
   @brief Inserts a single account into the database

   This query can be used to insert a single DataModel::Account object into the Database.
 */
class InsertAccount : public OpenTodoList::DataBase::StorageQuery
{
    Q_OBJECT
public:

    /**
       @brief Flags used to influence the insertion behavior of the account.
     */
    enum InsertFlag {
        NoFlags         =  0x00, //!< No flags
        MarkAsDirty     = 0x01,  //!< Mark the account as dirty
        MarkAsClean     = 0x02,  //!< Mark the account as clean (note that MarkAsDirty has precedence if both are used together)
        MarkAsDisposed  = 0x04   //!< Mark the account as disposed
    };

    Q_DECLARE_FLAGS( InsertFlags, InsertFlag )

    explicit InsertAccount( DataModel::Account *account, InsertFlags flags = NoFlags,
                            ResourceOwnership ownership = QueryIsOwner );
    virtual ~InsertAccount();

    // StorageQuery interface
    bool query(QString &query, QVariantMap &args) override;
    bool hasNext() const override;
    void newIdAvailable(const QVariant &id) override;
    void endRun() override;

signals:

public slots:

private:

    enum State {
        InsertAccountState,
        InsertMetaNameState,
        InsertMetaState,
        RemoveExtraMetaState,
        DoneState
    };

    DataModel::Account *m_account;
    State               m_state;
    InsertFlags         m_flags;
    ResourceOwnership   m_ownership;
    bool                m_waitForId;


};

} // namespace Queries
} // namespace DataBase
} // namespace OpenTodoList

#endif // OPENTODOLIST_DATABASE_QUERIES_INSERTACCOUNT_H

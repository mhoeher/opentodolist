#ifndef OPENTODOLIST_DATABASE_QUERIES_INSERTTASK_H
#define OPENTODOLIST_DATABASE_QUERIES_INSERTTASK_H

#include "datamodel/task.h"

#include "database/storagequery.h"

namespace OpenTodoList {
namespace DataBase {
namespace Queries {

using namespace DataModel;

class InsertTask : public StorageQuery
{
    Q_OBJECT
public:
    explicit InsertTask( Task *task );

    // StorageQuery interface
    bool query(QString &query, QVariantMap &args) override;
    void newIdAvailable(const QVariant &id) override;
    void endRun() override;
    bool hasNext() const override;

signals:

public slots:

private:

    enum State {
        InsertTaskState,
        InsertMetaNamesState,
        InsertMetaValuesState,
        RemoveExtraMetaValuesState,
        FinishedState
    };

    Task *m_task;
    State m_state;
    bool  m_waitingForId;
};

} // namespace Queries
} // namespace DataBase
} // namespace OpenTodoList

#endif // OPENTODOLIST_DATABASE_QUERIES_INSERTTASK_H

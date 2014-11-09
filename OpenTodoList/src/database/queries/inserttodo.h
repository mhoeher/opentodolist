#ifndef OPENTODOLIST_DATABASE_QUERIES_INSERTTODO_H
#define OPENTODOLIST_DATABASE_QUERIES_INSERTTODO_H

#include "datamodel/todo.h"

#include "database/storagequery.h"

namespace OpenTodoList {
namespace DataBase {
namespace Queries {

using namespace DataModel;

class InsertTodo : public StorageQuery
{
    Q_OBJECT
public:
    explicit InsertTodo(Todo *todo);

    // StorageQuery interface
    bool query(QString &query, QVariantMap &args);
    void newIdAvailable(const QVariant &id);
    void endRun();
    bool hasNext() const;

signals:

public slots:

private:

    enum State {
        InsertTodoState,
        InsertMetaAttributeNameState,
        InsertMetaAttributeValueState,
        RemoveExtraneousMetaAttributesState,
        FinishedState
    };

    Todo *m_todo;
    State m_state;
    bool  m_waitForId;

};

} // namespace Queries
} // namespace DataBase
} // namespace OpenTodoList

#endif // OPENTODOLIST_DATABASE_QUERIES_INSERTTODO_H

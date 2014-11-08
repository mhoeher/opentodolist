#ifndef OPENTODOLIST_DATABASE_QUERY_INSERTTODOLIST_H
#define OPENTODOLIST_DATABASE_QUERY_INSERTTODOLIST_H

#include "datamodel/todolist.h"

#include "database/storagequery.h"

namespace OpenTodoList {
namespace DataBase {
namespace Queries {

using namespace DataModel;

class InsertTodoList : public StorageQuery
{
    Q_OBJECT
public:
    explicit InsertTodoList( TodoList *todoList );

    // StorageQuery interface
    bool query(QString &query, QVariantMap &args);
    void newIdAvailable(const QVariant &id);
    void endRun();
    bool hasNext() const;

signals:

public slots:

private:

    enum State {
        InsertTodoListState,
        InsertMetaAttributeNamesState,
        InsertMetaAttributeValuesState,
        RemoveExtraneousMetaAttributeValuesState,
        FinishedState
    };

    State     m_state;
    TodoList *m_todoList;
    bool      m_waitForId;

};

} // namespace Queries
} // namespace DataBase
} // namespace OpenTodoList

#endif // OPENTODOLIST_DATABASE_QUERY_INSERTTODOLIST_H

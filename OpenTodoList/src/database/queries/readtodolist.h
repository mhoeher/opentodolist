#ifndef OPENTODOLIST_DATABASE_QUERIES_READTODOLIST_H
#define OPENTODOLIST_DATABASE_QUERIES_READTODOLIST_H

#include "datamodel/todolist.h"

#include "database/storagequery.h"

namespace OpenTodoList {
namespace DataBase {
namespace Queries {

using namespace DataModel;

class ReadTodoList : public StorageQuery
{
    Q_OBJECT
public:
    explicit ReadTodoList();

    QList<TodoList *> todoLists() const;

    // StorageQuery interface
    bool query(QString &query, QVariantMap &args) override;
    void recordAvailable(const QVariantMap &record) override;
    void endRun() override;

signals:

    void readTodoList( const QVariant &todoList );

public slots:

private:

    QList<TodoList*> m_todoLists;
    TodoList        *m_currentTodoList;

};

} // namespace Queries
} // namespace DataBase
} // namespace OpenTodoList

#endif // OPENTODOLIST_DATABASE_QUERIES_READTODOLIST_H

#ifndef OPENTODOLIST_DATABASE_QUERIES_READTODO_H
#define OPENTODOLIST_DATABASE_QUERIES_READTODO_H

#include "datamodel/todo.h"

#include "database/storagequery.h"

namespace OpenTodoList {
namespace DataBase {
namespace Queries {

using namespace DataModel;

class ReadTodo : public StorageQuery
{
    Q_OBJECT
public:
    explicit ReadTodo();

    // StorageQuery interface
    bool query(QString &query, QVariantMap &args) override;
    void recordAvailable(const QVariantMap &record) override;
    void endRun() override;

    QList<Todo *> todos() const;

    QUuid todoListUuid() const;
    void setTodoListUuid(const QUuid &todoListUuid);

    QDateTime minDueDate() const;
    void setMinDueDate(const QDateTime &minDueDate);

    QDateTime maxDueDate() const;
    void setMaxDueDate(const QDateTime &maxDueDate);

    bool showDone() const;
    void setShowDone(bool showDone);

signals:

    void readTodo( const QVariant &todo );

public slots:

private:
    QList<Todo *> m_todos;
    Todo *m_currentTodo;

    // Filtering
    QUuid m_todoListUuid;
    QDateTime m_minDueDate;
    QDateTime m_maxDueDate;
    bool m_showDone;

};

} // namespace Queries
} // namespace DataBase
} // namespace OpenTodoList

#endif // OPENTODOLIST_DATABASE_QUERIES_READTODO_H

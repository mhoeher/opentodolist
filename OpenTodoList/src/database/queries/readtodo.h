#ifndef OPENTODOLIST_DATABASE_QUERIES_READTODO_H
#define OPENTODOLIST_DATABASE_QUERIES_READTODO_H

#include "datamodel/todo.h"

#include "database/queries/private/readobject.h"
#include "database/storagequery.h"

namespace OpenTodoList {
namespace DataBase {
namespace Queries {

using namespace Private;
using namespace DataModel;

class ReadTodo : public ReadObject<Todo>
{
    Q_OBJECT
public:
    explicit ReadTodo();

    QList<Todo *> todos() const;

    QDateTime minDueDate() const;
    void setMinDueDate(const QDateTime &minDueDate);

    QDateTime maxDueDate() const;
    void setMaxDueDate(const QDateTime &maxDueDate);

    bool showDone() const;
    void setShowDone(bool showDone);

    QString filter() const;
    void setFilter(const QString &filter);

signals:

    void readTodo( const QVariant &todo );

public slots:

protected:
    // ReadObject interface
    ConditionList generatedConditions() const;

private:

    // Filtering
    QDateTime m_minDueDate;
    QDateTime m_maxDueDate;
    bool m_showDone;
    QString m_filter;


};

} // namespace Queries
} // namespace DataBase
} // namespace OpenTodoList

#endif // OPENTODOLIST_DATABASE_QUERIES_READTODO_H

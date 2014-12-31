#ifndef OPENTODOLIST_DATABASE_QUERIES_READTODOLIST_H
#define OPENTODOLIST_DATABASE_QUERIES_READTODOLIST_H

#include "datamodel/todolist.h"

#include "database/storagequery.h"
#include "database/queries/private/readobject.h"

namespace OpenTodoList {
namespace DataBase {
namespace Queries {

using namespace Private;
using namespace DataModel;

class ReadTodoList : public ReadObject<TodoList>
{
    Q_OBJECT
public:
    explicit ReadTodoList();

    QList<TodoList *> todoLists() const;

signals:

    void readTodoList( const QVariant &todoList );

public slots:

private:

};

} // namespace Queries
} // namespace DataBase
} // namespace OpenTodoList

#endif // OPENTODOLIST_DATABASE_QUERIES_READTODOLIST_H

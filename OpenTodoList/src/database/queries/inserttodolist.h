#ifndef OPENTODOLIST_DATABASE_QUERY_INSERTTODOLIST_H
#define OPENTODOLIST_DATABASE_QUERY_INSERTTODOLIST_H

#include "datamodel/todolist.h"

#include "database/storagequery.h"
#include "database/queries/private/insertobject.h"

namespace OpenTodoList {
namespace DataBase {
namespace Queries {

using namespace Private;
using namespace DataModel;

class InsertTodoList : public InsertObject<TodoList>
{
    Q_OBJECT
public:
    explicit InsertTodoList(TodoList *todoList , bool update);

signals:

public slots:

private:

};

} // namespace Queries
} // namespace DataBase
} // namespace OpenTodoList

#endif // OPENTODOLIST_DATABASE_QUERY_INSERTTODOLIST_H

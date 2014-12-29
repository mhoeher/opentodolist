#ifndef OPENTODOLIST_DATABASE_QUERIES_INSERTTODO_H
#define OPENTODOLIST_DATABASE_QUERIES_INSERTTODO_H

#include "datamodel/todo.h"

#include "database/storagequery.h"
#include "database/queries/private/insertobject.h"

namespace OpenTodoList {
namespace DataBase {
namespace Queries {

using namespace Private;
using namespace DataModel;

class InsertTodo : public InsertObject<Todo>
{
    Q_OBJECT
public:
    explicit InsertTodo(Todo *todo, bool update);

signals:

public slots:

private:

};

} // namespace Queries
} // namespace DataBase
} // namespace OpenTodoList

#endif // OPENTODOLIST_DATABASE_QUERIES_INSERTTODO_H

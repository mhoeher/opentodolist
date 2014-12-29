#ifndef OPENTODOLIST_DATABASE_QUERIES_INSERTTASK_H
#define OPENTODOLIST_DATABASE_QUERIES_INSERTTASK_H

#include "datamodel/task.h"

#include "database/storagequery.h"
#include "database/queries/private/insertobject.h"

namespace OpenTodoList {
namespace DataBase {
namespace Queries {

using namespace DataModel;
using namespace Private;

class InsertTask : public InsertObject<Task>
{
    Q_OBJECT
public:
    explicit InsertTask(Task *task , bool update);

signals:

public slots:

private:

};

} // namespace Queries
} // namespace DataBase
} // namespace OpenTodoList

#endif // OPENTODOLIST_DATABASE_QUERIES_INSERTTASK_H

#ifndef OPENTODOLIST_DATABASE_QUERIES_READTASK_H
#define OPENTODOLIST_DATABASE_QUERIES_READTASK_H

#include "database/queries/private/readobject.h"
#include "database/storagequery.h"

#include "datamodel/task.h"

#include <QList>

namespace OpenTodoList {
namespace DataBase {
namespace Queries {

using namespace Private;
using namespace DataModel;

class ReadTask : public ReadObject<Task>
{
  Q_OBJECT
public:
  explicit ReadTask();

  QList<Task *> tasks() const;

  int todoId() const;
  void setTodoId(int todoId);

signals:

  void readTask( QVariant task );

public slots:

private:

};

} // namespace Queries
} // namespace DataBase
} // namespace OpenTodoList

#endif // OPENTODOLIST_DATABASE_QUERIES_READTASK_H

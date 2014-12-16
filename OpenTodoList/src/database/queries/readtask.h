#ifndef OPENTODOLIST_DATABASE_QUERIES_READTASK_H
#define OPENTODOLIST_DATABASE_QUERIES_READTASK_H

#include "database/storagequery.h"

#include "datamodel/task.h"

#include <QList>

namespace OpenTodoList {
namespace DataBase {
namespace Queries {

using namespace DataModel;

class ReadTask : public StorageQuery
{
  Q_OBJECT
public:
  explicit ReadTask();

  // StorageQuery interface
  bool query(QString &query, QVariantMap &args) override;
  void recordAvailable(const QVariantMap &record) override;
  void endRun() override;

  QList<Task *> tasks() const;

  int todoId() const;
  void setTodoId(int todoId);

signals:

  void readTask( QVariant task );

public slots:

private:

  QList<Task *> m_tasks;
  int           m_todoId;

  Task         *m_currentTask;

};

} // namespace Queries
} // namespace DataBase
} // namespace OpenTodoList

#endif // OPENTODOLIST_DATABASE_QUERIES_READTASK_H

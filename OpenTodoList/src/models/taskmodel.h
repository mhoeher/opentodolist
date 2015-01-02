#ifndef OPENTODOLIST_MODELS_TASKMODEL_H
#define OPENTODOLIST_MODELS_TASKMODEL_H

#include "models/private/objectmodel.h"

#include "database/database.h"
#include "datamodel/task.h"
#include "datamodel/todo.h"

namespace OpenTodoList {
namespace Models {

using namespace DataBase;
using namespace DataModel;
using namespace Models::Private;

class TaskModel : public ObjectModel
{
  Q_OBJECT
  Q_PROPERTY(OpenTodoList::DataModel::Todo* todo READ todo WRITE setTodo NOTIFY todoChanged)
public:
  explicit TaskModel(QObject *parent = 0);

  Todo* todo() const;
  void setTodo(Todo* todo);

signals:

  void todoChanged();

public slots:

private:

  QPointer< Todo > m_todo;

protected:
  // ObjectModel interface
  void connectToDatabase() override;
  void disconnectFromDatabase() override;
  StorageQuery *createQuery() const override;

private slots:
  void addTask( const QVariant &task );
  void removeTask( const QVariant &task );

};

} // namespace Models
} // namespace OpenTodoList

#endif // OPENTODOLIST_MODELS_TASKMODEL_H

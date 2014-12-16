#ifndef OPENTODOLIST_MODELS_TASKMODEL_H
#define OPENTODOLIST_MODELS_TASKMODEL_H

#include <QAbstractListModel>

#include "database/database.h"
#include "datamodel/task.h"
#include "datamodel/todo.h"

namespace OpenTodoList {
namespace Models {

using namespace DataBase;
using namespace DataModel;

class TaskModel : public QAbstractListModel
{
  Q_OBJECT
  Q_PROPERTY(OpenTodoList::DataBase::Database* database READ database WRITE setDatabase NOTIFY databaseChanged)
  Q_PROPERTY(OpenTodoList::DataModel::Todo* todo READ todo WRITE setTodo NOTIFY todoChanged)
public:
  explicit TaskModel(QObject *parent = 0);

  Database *database() const;
  void setDatabase(Database *database);

  Todo* todo() const;
  void setTodo(Todo* todo);

  // QAbstractItemModel interface
  int rowCount(const QModelIndex &parent) const override;
  QVariant data(const QModelIndex &index, int role) const override;

signals:

  void databaseChanged();
  void todoChanged();

public slots:

  void refresh();

private:

  Database *m_database;
  QVector< Task* > m_tasks;

  QPointer< Todo > m_todo;

private slots:
  void addTask( const QVariant &task );

};

} // namespace Models
} // namespace OpenTodoList

#endif // OPENTODOLIST_MODELS_TASKMODEL_H

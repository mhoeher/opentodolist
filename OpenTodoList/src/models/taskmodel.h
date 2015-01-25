/*
 *  OpenTodoList - A todo and task manager
 *  Copyright (C) 2014 - 2015  Martin Höher <martin@rpdev.net>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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
  Q_ENUMS(MoveTaskMode)
  Q_PROPERTY(OpenTodoList::DataModel::Todo* todo READ todo WRITE setTodo NOTIFY todoChanged)
public:

  enum MoveTaskMode {
    MoveTaskBefore,
    MoveTaskAfter
  };

  explicit TaskModel(QObject *parent = 0);

  Todo* todo() const;
  void setTodo(Todo* todo);

signals:

  void todoChanged();

public slots:
  void moveTask( Task *task, MoveTaskMode mode, Task *target );

private:

  QPointer< Todo > m_todo;

protected:
  // ObjectModel interface
  void connectToDatabase() override;
  void disconnectFromDatabase() override;
  StorageQuery *createQuery() const override;
  int compareObjects(QObject *left, QObject *right) const;

private slots:
  void addTask( const QVariant &task );
  void removeTask( const QVariant &task );
};

} // namespace Models
} // namespace OpenTodoList

#endif // OPENTODOLIST_MODELS_TASKMODEL_H

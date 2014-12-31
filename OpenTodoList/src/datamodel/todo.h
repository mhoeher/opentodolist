/*
 *  OpenTodoList - A todo and task manager
 *  Copyright (C) 2013, 2014  Martin Höher <martin@rpdev.net>
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

#ifndef ABSTRACTTODO_H
#define ABSTRACTTODO_H

#include "core/opentodolistinterfaces.h"

#include "datamodel/todolist.h"

#include <QObject>
#include <QPointer>

namespace OpenTodoList {

namespace DataModel {

class Todo : public QObject, public ITodo
{
  Q_OBJECT
  Q_CLASSINFO( "id", "id" ) //!< The name of the ID property
  Q_CLASSINFO( "uuid", "uuid" ) //!< The name of the UUID property
  Q_PROPERTY(int id READ id NOTIFY idChanged)
  Q_PROPERTY(bool hasId READ hasId NOTIFY idChanged)
  Q_PROPERTY(QUuid uuid READ uuid WRITE setUuid NOTIFY uuidChanged)
  Q_PROPERTY(double weight READ weight WRITE setWeight NOTIFY weightChanged )
  Q_PROPERTY(bool isDone READ done WRITE setDone NOTIFY doneChanged )
  Q_PROPERTY(int priority READ priority WRITE setPriority NOTIFY priorityChanged )
  Q_PROPERTY(QUuid todoList READ todoList WRITE setTodoList NOTIFY todoListUuidChanged )
  Q_PROPERTY(QDateTime dueDate READ dueDate WRITE setDueDate NOTIFY dueDateChanged )
  Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged )
  Q_PROPERTY(QString description READ description WRITE setDescription NOTIFY descriptionChanged )
  Q_PROPERTY(QVariantMap metaAttributes READ metaAttributes NOTIFY metaAttributesChanged)

public:

  /**
     @brief The type of object containing todos
   */
  typedef TodoList ContainerType;

  explicit Todo( QObject* parent = 0 );
  virtual ~Todo();

  int id() const;
  void setId(int id);
  bool hasId() const;

  // ITodo interface
  const QUuid &uuid() const override;
  void setUuid(const QUuid &uuid) override;
  double weight() const override;
  void setWeight(double weight) override;
  bool done() const override;
  void setDone(bool done) override;
  int priority() const override;
  void setPriority(int priority) override;
  const QUuid& todoList() const override;
  void setTodoList(const QUuid &uuid) override;
  const QDateTime &dueDate() const override;
  void setDueDate(const QDateTime &dueOn) override;
  const QString &title() const override;
  void setTitle(const QString &title) override;
  const QString &description() const override;
  void setDescription(const QString &description) override;
  QVariantMap metaAttributes() const override;
  void setMetaAttributes( const QVariantMap &metaAttributes ) override;

  Q_INVOKABLE QVariant toVariant() const;
  Q_INVOKABLE void fromVariant( const QVariant &todo );

  int dirty() const;
  void setDirty(int dirty);

  bool disposed() const;
  void setDisposed(bool disposed);

signals:

  void idChanged();
  void uuidChanged();
  void weightChanged();
  void doneChanged();
  void priorityChanged();
  void dueDateChanged();
  void titleChanged();
  void descriptionChanged();
  void todoListUuidChanged();
  void metaAttributesChanged();

  void changed();

public slots:

  void toggle();

protected:

private:

  void setupTodo();

private:

  // ITodo properties
  int                     m_id;
  bool                    m_hasId;
  QUuid                   m_uuid;
  double                  m_weight;
  bool                    m_done;
  int                     m_priority;
  QUuid                   m_todoListUuid;
  QDateTime               m_dueDate;
  QString                 m_title;
  QString                 m_description;
  QVariantMap             m_metaAttributes;
  int                     m_dirty;
  bool                    m_disposed;

};

} /* DataModel */

} /* OpenTodoList */

#endif // ABSTRACTTODO_H

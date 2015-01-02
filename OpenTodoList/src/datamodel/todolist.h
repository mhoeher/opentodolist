/*
 *  OpenTodoList - A todo and task manager
 *  Copyright (C) 2013 - 2015  Martin Höher <martin@rpdev.net>
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

#ifndef ABSTRACTTODOLIST_H
#define ABSTRACTTODOLIST_H

#include "core/opentodolistinterfaces.h"
#include "datamodel/account.h"

#include <QList>
#include <QObject>
#include <QObjectList>
#include <QPointer>
#include <QVariant>
#include <QVariantMap>

namespace OpenTodoList {

namespace DataModel {

class TodoListLibrary;

class TodoList : public QObject, public ITodoList
{
  Q_OBJECT
  Q_CLASSINFO( "id", "id" ) //!< The name of the ID property
  Q_CLASSINFO( "uuid", "uuid" ) //!< The name of the UUID property
  Q_PROPERTY(int id READ id NOTIFY idChanged)
  Q_PROPERTY(bool hasId READ hasId NOTIFY idChanged)
  Q_PROPERTY(QUuid uuid READ uuid WRITE setUuid NOTIFY uuidChanged)
  Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
  Q_PROPERTY(QVariantMap metaAttributes READ metaAttributes NOTIFY metaAttributesChanged)
  Q_PROPERTY(QUuid account READ account WRITE setAccount NOTIFY accountUuidChanged)

public:

  /**
     @brief The type of objects containing todo lists
   */
  typedef Account ContainerType;

  explicit TodoList( QObject *parent = 0 );
  virtual ~TodoList();

  // ITodoList interface
  const QUuid &uuid() const override;
  void setUuid(const QUuid &uuid) override;
  const QString &name() const override;
  void setName(const QString &name) override;
  QVariantMap metaAttributes() const override;
  void setMetaAttributes( const QVariantMap &metaAttributes ) override;
  QUuid account() const;
  void setAccount(const QUuid &uuid);

  int id() const;
  void setId( int id );
  bool hasId() const;

  QVariant toVariant() const;
  void fromVariant( const QVariant &todolist );

  int dirty() const;
  void setDirty(int dirty);

  bool disposed() const;
  void setDisposed(bool disposed);

signals:

  void idChanged();
  void uuidChanged();
  void nameChanged();
  void metaAttributesChanged();
  void accountUuidChanged();

  void changed();

private:
  int m_id;
  bool m_hasId;
  QUuid m_uuid;
  QString m_name;
  QVariantMap m_metaAttributes;
  QUuid m_accountUuid;
  int m_dirty;
  bool m_disposed;
  bool m_loading;

public slots:

private slots:
  void emitChanged();

};

} /* DataModel */

} /* OpenTodoList */

#endif // ABSTRACTTODOLIST_H

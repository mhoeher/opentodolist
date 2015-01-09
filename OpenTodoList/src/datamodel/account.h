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

#ifndef ACCOUNT_H
#define ACCOUNT_H

#include "core/opentodolistinterfaces.h"

#include "datamodel/backend.h"

#include <QObject>

namespace OpenTodoList {
namespace DataModel {

class Account : public QObject, public IAccount
{
  Q_OBJECT
  Q_CLASSINFO( "id", "id" ) //!< The name of the ID property
  Q_CLASSINFO( "uuid", "uuid" ) //!< The name of the UUID property
  Q_CLASSINFO( "ContainerType", "Backend" ) //

  Q_PROPERTY(bool hasId READ hasId NOTIFY idChanged)
  Q_PROPERTY(int id READ id NOTIFY idChanged)
  Q_PROPERTY(QUuid uuid READ uuid WRITE setUuid NOTIFY uuidChanged)
  Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
  Q_PROPERTY(QString backend READ backend WRITE setBackend NOTIFY backendChanged)
  Q_PROPERTY(QVariantMap metaAttributes READ metaAttributes NOTIFY metaAttributesChanged)
public:

  /**
     @brief The type of object that contains accounts
   */
  typedef Backend ContainerType;

  explicit Account(QObject *parent = 0);

  bool hasId() const;
  int id() const;
  void setId( int id );
  int dirty() const;
  void setDirty( int dirty );
  bool disposed() const;
  void setDisposed( bool disposed );
  void setBackend( const QString &backend );

  // IAccount interface
  const QUuid &uuid() const override;
  void setUuid(const QUuid &uuid) override;
  const QString &name() const override;
  void setName(const QString &name) override;
  QString backend() const override;
  QVariantMap metaAttributes() const override;
  void setMetaAttributes(const QVariantMap &metaAttributes) override;

  Q_INVOKABLE QVariant toVariant() const;
  Q_INVOKABLE void fromVariant(const QVariant &account );


signals:

  void idChanged();
  void uuidChanged();
  void nameChanged();
  void backendChanged();
  void metaAttributesChanged();

  /**
       @brief The Account object has been changed

       This signal is emitted if one of the properties relevant to the frontend have been
       changed. It is not triggered for properties that are used internally by the database
       system only.
     */
  void changed();

public slots:

private:

  bool        m_hasId;
  int         m_id;
  QUuid       m_uuid;
  QString     m_name;
  QString     m_backend;
  int         m_dirty;
  bool        m_disposed;
  QVariantMap m_metaAttributes;

  bool        m_loading;

private slots:
  void emitChanged();

};

} /* namespace DataModel */
} /* namespace OpenTodoList */

#endif // ACCOUNT_H

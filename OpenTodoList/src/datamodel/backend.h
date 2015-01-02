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

#ifndef OPENTODOLIST_DATAMODEL_BACKEND_H
#define OPENTODOLIST_DATAMODEL_BACKEND_H

#include <QObject>

namespace OpenTodoList {
namespace DataModel {

/**
   @brief A class used to represent a backend

   The Backend class is used to represent storage backends (i.e. plugins implementing the
   OpenTodoList::IBackend interface) of the application.
 */
class Backend : public QObject
{
  Q_OBJECT
  Q_CLASSINFO( "id", "id" ) //!< The name of the ID property
  Q_CLASSINFO( "uuid", "name" ) //!< The name of the UUID property
  Q_PROPERTY(bool hasId READ hasId NOTIFY idChanged)
  Q_PROPERTY(int id READ id NOTIFY idChanged)
  Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
  Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
  Q_PROPERTY(QString description READ description WRITE setDescription NOTIFY descriptionChanged)
public:

  /**
     @brief The type of object that contains accounts

     @note This is a helper to indicate that backends are in fact NOT contained in other
           objects. Using std::is_empty should return true on it.
   */
  typedef struct {} ContainerType;

  explicit Backend(QObject *parent = 0);

  bool hasId() const;
  int id() const;
  void setId( int id );
  QString name() const;
  void setName( const QString &name );
  QString title() const;
  void setTitle( const QString &title );
  QString description() const;
  void setDescription( const QString &description );

  /**
     @brief Whether the backend has been disposed

     @note This is for convenience. This method returns always false (as backends are supposed to be
           non-disposable).
   */
  bool disposed() const { return false; }

  QVariant toVariant() const;
  void fromVariant( const QVariant &backend );

signals:

  void idChanged();
  void nameChanged();
  void titleChanged();
  void descriptionChanged();

  void changed();

public slots:

private:

  bool    m_hasId;
  int     m_id;
  QString m_name;
  QString m_title;
  QString m_description;

  bool    m_loading;

private slots:
  void emitChanged();

};

} // namespace DataModel
} // namespace OpenTodoList

#endif // OPENTODOLIST_DATAMODEL_BACKEND_H

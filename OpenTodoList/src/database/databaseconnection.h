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

#ifndef OPENTODOLIST_DATABASE_DATABASECONNECTION_H
#define OPENTODOLIST_DATABASE_DATABASECONNECTION_H

#include "datamodel/account.h"
#include "datamodel/todolist.h"
#include "datamodel/todo.h"
#include "datamodel/task.h"

#include "database/database.h"

namespace OpenTodoList {
namespace DataBase {

using namespace DataModel;

/**
   @brief A connection to the database

   This is a helper class that allows accessing the database from the front end part
   and insert, modify and dispose objects from it. In the simples cases.
   one can directly use the various methods provided to modify the database
   e.g. by adding a new object.

   Apart, one can also register objects in the connection. Registered objects
   can be synchronized between the registered object instance and the
   database.
 */
class DatabaseConnection : public QObject
{
  Q_OBJECT
  Q_ENUMS(UpdateFlags)
  Q_PROPERTY(OpenTodoList::DataBase::Database* database READ database WRITE setDatabase NOTIFY databaseChanged)
public:

  /**
     @brief Determines how updates in registered objects are handled

     The flags defined here can be used to influence how
     registered objects are treated with respec to
     updates.
   */
  enum UpdateFlags {
    /**
      No updates will be applied. This can be used to unregister a previously
      registered object.
      */
    NoUpdates        = 0,

    /**
      If the object gets updated in the database, apply the changes to the
      registered object instance as well.
      */
    DatabaseToObject = 0x01,

    /**
      If the object gets changed (i.e. its changed() signal is emitted),
      write back the object to the database.
      */
    ObjectToDatabase = 0x02,

    /**
      Keep database and registered object instance in sync. This is
      basically the same as using both the DatabaseToObject
      as well as ObjectToDatabase flags.
      */
    KeepSynced       = DatabaseToObject | ObjectToDatabase
  };

  explicit DatabaseConnection(QObject *parent = 0);
  ~DatabaseConnection();

  Database *database() const;
  void setDatabase(Database *database);

signals:

  void databaseChanged();

public slots:

  void insertAccount( Account *account );
  void insertTodoList( TodoList *todoList );
  void insertTodo( Todo *todo );
  void insertTask( Task *task );
  void disposeAccount( Account *account );
  void disposeTodoList( TodoList *todoList );
  void disposeTodo( Todo *todo );
  void disposeTask( Task *task );

  void registerAccount( Account *account, UpdateFlags updateFlags = KeepSynced );
  void registerTodoList( TodoList *todoList, UpdateFlags updateFlags = KeepSynced );
  void registerTodo( Todo *todo, UpdateFlags updateFlags = KeepSynced );
  void registerTask( Task *task, UpdateFlags updateFlags = KeepSynced );

private:

  Database *m_database;
  QMap<QObject*,UpdateFlags> m_registeredObjects;

  template<typename T>
  void registerObjectImpl( T *object, UpdateFlags updateFlags ) {
    if ( object ) {
      disconnect(object, &T::changed,
                 this, &DatabaseConnection::onObjectInstanceUpdate);
      m_registeredObjects.remove( object );
      if ( updateFlags != NoUpdates ) {
        m_registeredObjects.insert( object, updateFlags );
        connect(object, &T::changed,
                this, &DatabaseConnection::onObjectInstanceUpdate);
        connect( object, &QObject::destroyed, this, &DatabaseConnection::onObjectDestroyed );
      }
    }
  }

  template<typename T>
  void onDatabaseUpdateImpl(const QVariant &data) {
    T *tmp = new T();
    tmp->fromVariant( data );
    for ( auto it = m_registeredObjects.begin(); it != m_registeredObjects.end(); ++it ) {
      if ( it.value() & DatabaseToObject ) {
        T *instance = qobject_cast<T*>( it.key() );
        if ( instance ) {
          if ( instance->uuid() == tmp->uuid() ) {
            instance->fromVariant( data );
          }
        }
      }
    }
    delete tmp;
  }

private slots:

  void onObjectInstanceUpdate();
  void onDatabaseAccountUpdate(const QVariant &account);
  void onDatabaseTodoListUpdate(const QVariant &todoList);
  void onDatabaseTodoUpdate(const QVariant &todo);
  void onDatabaseTaskUpdate(const QVariant &task);
  void onObjectDestroyed(QObject *o);

};

} // namespace DataBase
} // namespace OpenTodoList

#endif // OPENTODOLIST_DATABASE_DATABASECONNECTION_H

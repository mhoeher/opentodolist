/*
 *  OpenTodoList - A todo and task manager
 *  Copyright (C) 2013  Martin Höher <martin@rpdev.net>
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

#ifndef OPENTODOLISTINTERFACES_H
#define OPENTODOLISTINTERFACES_H

#include <QDateTime>
#include <QObject>
#include <QUuid>
#include <QVariantMap>

namespace OpenTodoList {

/**
   @brief Interface describing an account

   This class is used to represent the interface an Account implements. An account
   is used to group several TodoList objects together, usually to determine where (physically)
   to store the appropriate data.
 */
class IAccount {

public:

    /**
       @brief Destructor
       @note Required to keep some compilers from complaining
     */
    virtual ~IAccount() {}

    /**
       @brief A unique ID identifying the account
     */
    virtual const QUuid& uuid() const = 0;

    /**
       @brief The name of the account
     */
    virtual const QString& name() const = 0;

    /**
       @brief Returns whether the account has been permanently deleted.

       This is intended to be used by the backend managing the account. If this returns true,
       the backend should remove any connection to the appropriate account. Depending on the
       concrete backend, this might mean that either physical data of the account has to be
       deleted or only the connection to a remote service is removed (but the data maintained
       on the server remains intact).

       Upon completion, the backend should call IDatabase::accountDeletionFinished().
     */
    virtual bool disposed() const = 0;

    /**
       @brief Sets the account name
     */
    virtual void setName( const QString &name ) = 0;

    /**
       @brief Returns whether this is the backend's default account.

       This returns whether the account is the default account for the backend. A default account
       is usually used when a backend supports no accounts. In this case, the default account
       acts as a virtual account. In such a scenario, certain actions are not allowed on the
       account, such as deleting it (as this would implicitly mean deletion of the backend).
     */
    virtual bool isBackendAccount() const = 0;

    /**
       @brief Sets or unsets this as an backend account
       @sa isBackendAccount()
     */
    virtual void setBackendAccount( bool backendAccount ) = 0;

    /**
       @brief The date and time when the account has been modified the last time
     */
    virtual QDateTime lastModificationTime() const = 0;

    /**
       @brief Sets the date and time of the last modification to @p dateTime
     */
    virtual void setLastModificationTime( const QDateTime &dateTime ) = 0;

};

/**
  @brief Interface describing a todo list

  This interface is used to represent a single todo list.
  */
class ITodoList {

public:

    /**
       @brief Destructor
       @note This is required to stop some compilers from complaining.
     */
    virtual ~ITodoList() {}

    /**
       @brief Returns the universally unique ID of the todo list
     */
    virtual const QUuid& uuid() const = 0;
    /**
       @brief Sets the universally unique ID of the todo list.
       @param uuid The new UUID to be used by the todo list.
     */
    virtual void setUuid( const QUuid &uuid ) = 0;

    /**
       @brief The name of the todo list.
     */
    virtual const QString& name() const = 0;
    /**
       @brief Sets the name of the todo list.
       @param name
     */
    virtual void setName( const QString &name ) = 0;

    /**
       @brief Returns whether todo list is marked as deleted
     */
    virtual bool deleted() const = 0;

    /**
       @brief Mark the todo list as deleted
     */
    virtual void setDeleted( bool deleted ) = 0;

    /**
       @brief Returns whether the todo list has been deleted permanently.

       This is supposed to be used by the backend maintaining the
       todo list. If this returns true, the backend shall permanently remove
       the todo list (and any contained todos) and after that, call
       IDatabase::todoListDeletionFinished().
     */
    virtual bool disposed() const = 0;

    /**
       @brief Returns the list of meta attributes of the todo list.
     */
    virtual const QStringList metaAttributeKeys() const = 0;
    /**
       @brief Returns the value of the meta attribute @p key
     */
    virtual const QVariant metaAttribute( const QString &key ) const = 0;
    /**
       @brief Sets the value of the meta attribute @p key to @p value
     */
    virtual void setMetaAttribute( const QString &key, const QVariant &value ) = 0;
    /**
       @brief Returns true if the todo list has the meta attribute @p key
     */
    virtual bool hasMetaAttribute( const QString &key ) const = 0;
    /**
       @brief Removes the meta attribute @p key from the todo list.
     */
    virtual void deleteMetaAttribute( const QString &key ) = 0;

    /**
       @brief The date and time when the account has been modified the last time
     */
    virtual QDateTime lastModificationTime() const = 0;

    /**
       @brief Sets the date and time of the last modification to @p dateTime
     */
    virtual void setLastModificationTime( const QDateTime &dateTime ) = 0;

};

/**
  @brief Interface used to represent todos
  */
class ITodo {

public:

    /**
       @brief Destructor
       @note Required to keep some compilers from complaining.
     */
    virtual ~ITodo() {}

    /**
       @brief Returns the universally unique ID of the todo.
     */
    virtual const QUuid& uuid() const = 0;
    /**
       @brief Sets the universally unique ID of the todo to @p uuid.
     */
    virtual void setUuid( const QUuid &uuid ) = 0;


    /**
       @brief Returns the weight of the todo.

       The weight is used for manually ordering todos within their todo list or parent todo.
     */
    virtual double weight() const = 0;
    /**
       @brief Sets the @p weight of the todo.
     */
    virtual void setWeight( double weight ) = 0;

    /**
       @brief Returns the progress of the todo.

       The progress is a value between 0 and 100, where 100 means the todo
       is done and values between 0 and 99 inclusively that the todo is not yet finished.
     */
    virtual int progress() const = 0;
    /**
       @brief Sets the @p progress of the todo.
     */
    virtual void setProgress( int progress ) = 0;

    /**
       @brief Returns the priority of the todo.

       This returns the priority of the todo. The priority is a value between -1 and 10.
       Values between 0 and 10 mean that the priority is set, where 10 is the highest possible
       proprity and 0 the lowest possible one. The special value -1 means that no priorty at all
       has been assigned.
     */
    virtual int priority() const = 0;
    /**
       @brief Sets the @p priority of the todo.
     */
    virtual void setPriority( int priority ) = 0;

    /**
       @brief Returns the uuid of the parent todo (or a null one if the todo has no parent todo).
     */
    virtual const QUuid& parentTodoUuid() const = 0;
    /**
       @brief Makes this todo a child of the todo with the given @p uuid.
     */
    virtual void setParentTodoUuid( const QUuid &uuid ) = 0;

    /**
       @brief Returns the uuid of the todo list the todo is in.
     */
    virtual const QUuid& todoListUuid() const = 0;
    /**
       @brief Sets the @p uuid of the todo list the todo is in.
     */
    virtual void setTodoListUuid( const QUuid &uuid ) = 0;

    /**
       @brief Return the date and time the todo is due on.
     */
    virtual const QDateTime& dueDate() const = 0;
    /**
       @brief Sets the date and time the todo is @p dueOn.
     */
    virtual void setDueDate( const QDateTime& dueOn ) = 0;

    /**
       @brief Returns the title of the todo.
     */
    virtual const QString& title() const = 0;
    /**
       @brief Sets the @p title of the todo.
     */
    virtual void setTitle( const QString &title ) = 0;

    /**
       @brief Returns the description of the todo (potentially as an HTML formatted string).
     */
    virtual const QString &description() const = 0;
    /**
       @brief Sets the @p description of the todo.
     */
    virtual void setDescription( const QString& description ) = 0;

    /**
       @brief Returns true of the todo has been moved to the trash.
     */
    virtual bool isDeleted() const = 0;
    /**
       @brief Set whether the todo @p isDeleted or not (aka moved to trash).
     */
    virtual void setDeleted( bool isDeleted ) = 0;

    /**
       @brief Returns whether the todo is deleted permanently.

       If this returns true, the backend shall delete all references to the
       todo (including any sub-todo) and afterwards call
       IDatabase::todoDeletionFinished().
     */
    virtual bool disposed() const = 0;

    /**
       @brief Returns the list of meta attributes of the todo.
     */
    virtual const QStringList metaAttributeKeys() const = 0;
    /**
       @brief Returns the value of the meta attribute @p key.
     */
    virtual const QVariant metaAttribute( const QString &key ) const = 0;
    /**
       @brief Sets the value of the meta attribute @p key to @p value
     */
    virtual void setMetaAttribute( const QString &key, const QVariant &value ) = 0;
    /**
       @brief Returns true if the todo has the meta attribute @p key
     */
    virtual bool hasMetaAttribute( const QString &key ) const = 0;
    /**
       @brief Removes the meta attribute @p key from the todo .
     */
    virtual void deleteMetaAttribute( const QString &key ) = 0;

    /**
       @brief The date and time when the account has been modified the last time
     */
    virtual QDateTime lastModificationTime() const = 0;

    /**
       @brief Sets the date and time of the last modification to @p dateTime
     */
    virtual void setLastModificationTime( const QDateTime &dateTime ) = 0;

};

/**
   @brief Interface for the central todo database

   This interface provides means for accessing the central todo database
   of the application. It is passed to storage backends which can utilize it
   to push data into the database.
 */
class IDatabase {
public:

    /**
       @brief Destructor

       Required for some compilers.
     */
    virtual ~IDatabase() {}

    /**
       @brief Adds or updates the @p account in the database
     */
    virtual bool insertAccount( const IAccount *account ) = 0;

    /**
       @brief Adds or updates a todo list

       This will either add the todo @p list to the database or update it (if
       a list with the same TodoListStruct::id is already in the database).
       @return True if the operation was successfull or false otherwise.
     */
    virtual bool insertTodoList( const ITodoList *list ) = 0;

    /**
       @brief Adds or updates a todo

       This will either add the @p todo to the database or update it
       (if another record with the same TodoStruct::id is already in the
       database).
       @return True if the operation was successfull or false otherwise.
     */
    virtual bool insertTodo( const ITodo *todo ) = 0;

    /**
       @brief Deletes the @p account from the database.
     */
    virtual bool deleteAccount( const IAccount *account ) = 0;

    /**
       @brief Removes a todo list from the database

       This will remove the todo @p list from the database. The list is
       identified by its TodoListStruct::id.
       @return True in case the operation succeeded or false otherwise.
     */
    virtual bool deleteTodoList( const ITodoList *list ) = 0;

    /**
       @brief Removes a todo from the database

       This removes the @p todo from the database. The todo is identified by its
       TodoStruct::id.
       @return True if the operation was successfull or false otherwise.
     */
    virtual bool deleteTodo( const ITodo *todo ) = 0;

    /**
       @brief Creates a temporary account
       @note The caller of this method is responsible to delete the created object.
     */
    virtual IAccount* createAccount() = 0;

    /**
       @brief Creates a new todo list

       This method is supposed to be used by storage backends. It allows to create a
       temporary todo list object, fill it and pass it to the database to persist the
       settings. It is not supposed to be used in the actual application frameworks.

       @note The caller of this method is responsible to delete the created object.
     */
    virtual ITodoList* createTodoList() = 0;

    /**
       @brief Creates a new todo

       This method is supposed to be used by storage backends. It allows
       to create a temporary todo object, fill it and pass it to the database for
       persisting the todo. It is not supposed to be used in the application frameworks.

       @note The caller of this method is responsible to delete the created object.
     */
    virtual ITodo* createTodo() = 0;
};

/**
   @brief An interface that is implemented by storage backends

   This interface defines how a storage backend is structured. It provides
   several groups of methods:

   Some methods are used for setting up the backend (such as setDatabase). They
   are required to ensure a bi-directional communication between the core
   application and the backend.

   Another set of methods communicates information about the backend itself.
   This is mainly for presentation purposes in the frontend.

   Part of the mesthods are used for object life cycle management. These methods
   are invoked by the application on start up or shut down to let the backend
   initialize and finalize data structures as needed.

   Finally, some methods are present that let the application tell the backend
   about changed objects (todo lists, todos...) that belong to that backend.
   The backend can then persist these objects as required.
 */
class IBackend {

public:

    /**
       @brief Destructor

       Required for some compilers.
     */
    virtual ~IBackend() {}

    /**
       @brief Sets the todo list database to be used by the backend

       This sets the @p database that shall be used by the backend.
     */
    virtual void setDatabase( IDatabase* database ) = 0;

    /**
       @brief Sets the directory for locally stored files

       Sets the @p directory where the backend should put local files into. The
       backend might use this directory for storing any persistent files into.
     */
    virtual void setLocalStorageDirectory( const QString& directory ) = 0;

    /**
       @brief The identifier of the backend

       Returns a string that identifies the backend. Two different backends
       must use different IDs.
     */
    virtual QString id() const = 0;

    /**
       @brief The name of the backend

       Returns the name of the interface. This is used when presenting the
       backend in listings.
     */
    virtual QString name() const = 0;

    /**
       @brief Returns a description for the backend

       Returns a text that describes the backend. This might be used when
       the backend allows creation of new todo lists. In this case, the text
       might be displayed in a frontend to let the user know the specifics of
       a todo list created with with backend.
       @return
     */
    virtual QString description() const = 0;

    /**
       @brief The backend shall start

       This method is called once by the application when the backend is to be
       started.

       @returns True when starting the backend was successfull or
        false otherwise.
     */
    virtual bool start() = 0;

    /**
       @brief The backend shall stop

       This method is called once by the application when the backend is to be
       stopped.

       @return True in case the backend was successfully stopped or false
        otherwise.
     */
    virtual bool stop() = 0;

};

} /* namespace OpenTodoList */

Q_DECLARE_INTERFACE(OpenTodoList::IBackend, "net.rpdev.OpenTodoList.IBackend/1.0")


#endif // OPENTODOLISTINTERFACES_H

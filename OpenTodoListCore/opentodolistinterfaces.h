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


/**
  @brief Structure defining a single todo list

  This structure holds the properties of a todo list. It is used
  for communication between storage backends and the application.
  */
typedef struct {

    /**
       @brief A globally unique ID identifying the todo list
     */
    QUuid id;

    /**
       @brief The name of the todo list
     */
    QString name;

    /**
       @brief Arbitrary "meta" information about the todo list

       This property can be used to store arbitrary meta information about
       the list. This is intended to be used by the backends.
     */
    QVariantMap meta;

} TodoListStruct;

/**
  @brief Structure defining single todos

  This structure if used to describe a single todo. It is used in the
  communication between storage backends and the core application.
  */
typedef struct {

    /**
       @brief A globally unique ID identifying the todo
     */
    QUuid id;

    /**
       @brief The weight of the todo

       This describes the weight of the todo. This property might be used in
       the front end for manual sorting of todos. A todo with a higher weight
       is located at the bottom of lists whereas todos with less weight are on
       the top.
     */
    double weight;

    /**
       @brief The progress of the todo

       The progress of the todo. This value is between 0 and 100, where
       0 means the todo is not completed and 100 means it is completely
       done.
     */
    int progress;

    /**
       @brief The priority of the todo

       The priority of the todo. This value is between -1 and 10. Values
       from 0 to 10 describe priorities, where 10 is the highest
       priority and 0 is the lowest one. -1 is a special value which means
       no priority is assigned.
     */
    int priority;

    /**
       @brief The UUID of the parent todo

       This is the UUID of the parent todo of this todo. If the UUID is
       null, this todo has no parent (and thus is a top level todo within
       the todo list).
     */
    QUuid parentTodoId;

    /**
       @brief The UUID of the todo list

       This is the UUID of the todo list this todo is in. Note, that this
       must point to a valid UUID.
     */
    QUuid todoListId;

    /**
       @brief The due date of the todo

       This is the date this todo is due on.
     */
    QDateTime dueDate;

    /**
       @brief The title of the todo
     */
    QString title;

    /**
       @brief The description of the todo

       This is the description of the todo. It is used to provide
       additional information about the todo. The description might contain
       HTML markup.
     */
    QString description;

    /**
       @brief The deleted state of the todo

       If this is true, the todo is flagged as deleted. This is used to
       implement a deletion mechanism that allows the user to restore the
       todo.
     */
    bool deleted;

    /**
       @brief Arbitrary "meta" information

       This is intended to be used by the various backends to store whatever
       additional information is required about a single todo.
     */
    QVariantMap meta;

} TodoStruct;

/**
   @brief Interface for the central todo database

   This interface provides means for accessing the central todo database
   of the application. It is passed to storage backends which can utilize it
   to push data into the database.
 */
class TodoListDatabase {
public:

    /**
       @brief Destructor

       Required for some compilers.
     */
    virtual ~TodoListDatabase() {}

    /**
       @brief Adds or updates a todo list

       This will either add the todo @p list to the database or update it (if
       a list with the same TodoListStruct::id is already in the database).
       @return True if the operation was successfull or false otherwise.
     */
    virtual bool insertTodoList( const TodoListStruct &list ) = 0;

    /**
       @brief Adds or updates a todo

       This will either add the @p todo to the database or update it
       (if another record with the same TodoStruct::id is already in the
       database).
       @return True if the operation was successfull or false otherwise.
     */
    virtual bool insertTodo( const TodoStruct &list ) = 0;

    /**
       @brief Removes a todo list from the database

       This will remove the todo @p list from the database. The list is
       identified by its TodoListStruct::id.
       @return True in case the operation succeeded or false otherwise.
     */
    virtual bool deleteTodoList( const TodoListStruct &list ) = 0;

    /**
       @brief Removes a todo from the database

       This removes the @p todo from the database. The todo is identified by its
       TodoStruct::id.
       @return True if the operation was successfull or false otherwise.
     */
    virtual bool deleteTodo( const TodoStruct &todo ) = 0;

    /**
       @brief Creates an invalid todo list structure
     */
    virtual TodoListStruct nullTodoList() = 0;

    /**
       @brief Creates an invalid todo structure
     */
    virtual TodoStruct nullTodo() = 0;
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
class BackendInterface {

public:

    /**
       @brief Destructor

       Required for some compilers.
     */
    virtual ~BackendInterface() {}

    /**
       @brief Sets the todo list database to be used by the backend

       This sets the @p database that shall be used by the backend.
     */
    virtual void setDatabase( TodoListDatabase* database ) = 0;

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

    /**
       @brief A todo list has changed

       This method is called by the application to let the backend know when a
       todo @p list created by the backend has been changed by the application.

       @returns True in case handling of the change was successfull or false
        otherwise.

       @note The backend does not need to update the todo list in the database.
        This happens automatically before this method is invoked.
     */
    virtual bool notifyTodoListChanged( const TodoListStruct& list ) = 0;

    /**
       @brief A todo has changed

       This method is called by the application to let the backend know when a
       @p todo created by that backend has been changed by the application.

       @returns True in case handling the update was successfull or false
        otherwise.

       @note The backend does not need to update the todo in the database. This
        happens automatically before this method is invoked.
     */
    virtual bool notifyTodoChanged( const TodoStruct& todo ) = 0;

    /**
       @brief Delete a todo list

       This is called to notify that the user decided to delete a todo list. The backend
       should delete any resources belonging to the list as well.

       @note The backend will receive subsequent notifyTodoDeleted() calls for any
       todo in the list. These calls might safely be ignored if any resources occupied
       by these todos already get disposed when the deletion of the list is handled.
     */
    virtual void notifyTodoListDeleted( const TodoListStruct &list ) = 0;

    /**
       @brief Delete a todo

       This is called to notify that the user decided to delete a todo list. The backend
       shall delete any resources belonging to the todo.

       @note The backend will receive subsequent notifyTodoDeleted() calls for
       any (recursive) sub-todo of the todo. These calls might be ignored if deletion of resources
       already happens in this call.
     */
    virtual void notifyTodoDeleted( const TodoStruct &todo ) = 0;

    /**
       @brief Test whether we can add a todo

       This method shall return true if new todos can be created within the given @p list and below
       the specified parent @p todo. If the todo is to be created directly within the todo list,
       an invalid todo is passed as the parent todo.

       If adding new todos is not allowed, false shall be returned.

       @note This method must be implemented in a thread-safe way!
     */
    virtual bool canAddTodo( const TodoListStruct &list, const TodoStruct &todo ) = 0;

    /**
       @brief Creates a new todo

       This shall create a new todo in the specified todo @p list and (if set to a valid
       todo) the given parent @p todo. The initial values of the todo shall be picked up from the
       @p newTodo structure. However, the backend might decide to override values (esp. the ID,
       which shall be generated by the backend) by own values.
     */
    virtual void addTodo( TodoStruct newTodo, const TodoListStruct &list, const TodoStruct &todo ) = 0;

    /**
       @brief Can a new todo list be created by the user in this backend

       This returns true if the backend is able to create new todo lists as requested by the user.
       If false is returned, the backend is supposed to be "read only" in the sense that it
       only makes external todo lists available.

       @sa addTodoList
     */
    virtual bool canAddTodoList() = 0;

    /**
       @brief Create a new todo list

       Creates a new todo list based on the values in @p newList.
     */
    virtual void addTodoList( TodoListStruct newList ) = 0;
};

Q_DECLARE_INTERFACE(BackendInterface, "net.rpdev.OpenTodoList.BackendInterface/1.0")


#endif // OPENTODOLISTINTERFACES_H

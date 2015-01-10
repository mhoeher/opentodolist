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

#ifndef OPENTODOLISTINTERFACES_H
#define OPENTODOLISTINTERFACES_H

#include <QDateTime>
#include <QList>
#include <QObject>
#include <QUuid>
#include <QSet>
#include <QVariantMap>

namespace OpenTodoList {

/**
   @brief Account interface

   This interface is used to describe accounts. An Account is a connection to a Backend.
   Per type of backend, there might be one or more accounts. For example, if a backend implements
   access to a webservice for storing todo lists, then one might have several accounts to represent
   connections to different instances of this webservice. Other backends (which might e.g. store
   their data locally) might implement only a single account.

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
       @brief Sets the @p uuid of the account
     */
  virtual void setUuid( const QUuid &uuid ) = 0;

  /**
       @brief The name of the account
     */
  virtual const QString& name() const = 0;

  /**
       @brief Sets the account name
     */
  virtual void setName( const QString &name ) = 0;

  /**
       @brief Returns the name of the backend the account belongs to.

       @sa IBackend::name()
     */
  virtual QString backend() const = 0;

  /**
       @brief The meta attributes of the account
     */
  virtual QVariantMap metaAttributes() const = 0;

  /**
       @brief Sets the @p metaAttributes of the account
     */
  virtual void setMetaAttributes( const QVariantMap &metaAttributes ) = 0;

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
       @brief Returns the UUID of the account the todo list belongs to
     */
  virtual QUuid account() const = 0;

  /**
       @brief Sets the @p uuid of the account the toto list belongs to
     */
  virtual void setAccount( const QUuid &uuid ) = 0;

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
       @brief The meta attributes of the todo list
     */
  virtual QVariantMap metaAttributes() const = 0;

  /**
       @brief Sets the @p metaAttributes of the todo list
     */
  virtual void setMetaAttributes( const QVariantMap &metaAttributes ) = 0;

  /**
     @brief Insert a meta attribute

     This inserts the meta attribute @p name with the given @p value.
   */
  virtual void insertMetaAttribute( const QString &name, const QVariant &value ) = 0;

  /**
     @brief Check for existence of a meta attribute

     This returns true of there is a meta attribute with the given @p name.
   */
  virtual bool hasMetaAttribute( const QString &name ) const = 0;

  /**
     @brief Remove a meta attribute

     This removes the meta attribute with the given @p name if it exists.
   */
  virtual void removeMetaAttribute( const QString &name ) = 0;

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
       @brief Returns whether the todo is marked as done
     */
  virtual bool done() const = 0;

  /**
       @brief Marks the todo as @p done
     */
  virtual void setDone( bool done ) = 0;

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
       @brief Returns the uuid of the todo list the todo is in.
     */
  virtual const QUuid& todoList() const = 0;
  /**
       @brief Sets the @p uuid of the todo list the todo is in.
     */
  virtual void setTodoList( const QUuid &uuid ) = 0;

  /**
       @brief The meta attributes of the todo
     */
  virtual QVariantMap metaAttributes() const = 0;

  /**
       @brief Sets the @p metaAttributes of the todo
     */
  virtual void setMetaAttributes( const QVariantMap &metaAttributes ) = 0;

  /**
     @brief Insert a meta attribute

     This inserts the meta attribute @p name with the given @p value.
   */
  virtual void insertMetaAttribute( const QString &name, const QVariant &value ) = 0;

  /**
     @brief Check for existence of a meta attribute

     This returns true of there is a meta attribute with the given @p name.
   */
  virtual bool hasMetaAttribute( const QString &name ) const = 0;

  /**
     @brief Remove a meta attribute

     This removes the meta attribute with the given @p name if it exists.
   */
  virtual void removeMetaAttribute( const QString &name ) = 0;
};

/**
   @brief An interface describing sub-task of a todo
 */
class ITask {
public:

  /**
       @brief Destructor
       @note Required to keep some compilers from complaining.
     */
  virtual ~ITask() {}

  /**
       @brief The UUID of the task
     */
  virtual QUuid uuid() const = 0;

  /**
       @brief Sets the tasks @p uuid
     */
  virtual void setUuid( const QUuid &uuid ) = 0;

  /**
       @brief The title of the task
     */
  virtual QString title() const = 0;

  /**
       @brief Sets the tasks @p title
     */
  virtual void setTitle( const QString &title ) = 0;

  /**
       @brief Returns whether the task is marked as done
     */
  virtual bool done() const = 0;

  /**
       @brief Mark the task as @p done
     */
  virtual void setDone( bool done ) = 0;

  /**
       @brief The weight of the task
       @note The weight is used to order tasks
     */
  virtual double weight() const = 0;

  /**
       @brief Sets the @p weight of the task
     */
  virtual void setWeight( double weight ) = 0;

  /**
       @brief The meta attributes of the task
     */
  virtual QVariantMap metaAttributes() const = 0;

  /**
       @brief Sets the @p metaAttributes of the task
     */
  virtual void setMetaAttributes( const QVariantMap &metaAttributes ) = 0;

  /**
     @brief Insert a meta attribute

     This inserts the meta attribute @p name with the given @p value.
   */
  virtual void insertMetaAttribute( const QString &name, const QVariant &value ) = 0;

  /**
     @brief Check for existence of a meta attribute

     This returns true of there is a meta attribute with the given @p name.
   */
  virtual bool hasMetaAttribute( const QString &name ) const = 0;

  /**
     @brief Remove a meta attribute

     This removes the meta attribute with the given @p name if it exists.
   */
  virtual void removeMetaAttribute( const QString &name ) = 0;

  /**
       @brief Returns the UUID of the todo the task belongs to
     */
  virtual QUuid todo() const = 0;

  /**
       @brief Sets the @p uuid of the todo this task belongs to
     */
  virtual void setTodo( const QUuid &uuid ) = 0;

};

/**
   @brief Access to the application database

   This interface is used to provide access to the application's database to storage plugins
   (i.e. classes implementing the IBackend interface). It defines the protocol between application
   and backend which is used for the various use cases (insertion of objects, updating of objects,
   deletion og objects and so on).

   Detailed information about the protocol are available in @ref databaseproto.
 */
class IDatabase {
public:

  /**
     @brief Specify which objects to query
   */
  enum QueryFlags {
    QueryAny      = 0,    //!< If no flag is set, all objects will be retrieved
    QueryDisposed = 0x1,  //!< Only objects with the disposed flag set to true are retrieved
    QueryDirty    = 0x02  //!< Only objects with the dirty flag greater than 0 are retrieved
  };

  /**
       @brief Destructor

       Required for some compilers.
     */
  virtual ~IDatabase() {}

  /**
     @brief Create an account object

     This creates a new IAccount object and returns it. This shall be used whenever the
     backends wants to insert a new account. The returned object is supposed to be filled and
     passed to insertAccount() once. After that, the backend shall delete the object.
   */
  virtual IAccount* createAccount() = 0;

  /**
     @brief Creates a new todo list

     This method returns a new ITodoList object which can be filled out by the backend and
     put into the database via insertTodoList(). The returned todo list shall be destroyed after
     the insertion.
   */
  virtual ITodoList* createTodoList() = 0;

  /**
     @brief Creates a new todo

     This creates a new todo object and returns it. The backend can then fill out the object and
     put it into the database using insertTodo(). After this, the returned object shall be
     destroyed.
   */
  virtual ITodo* createTodo() = 0;

  /**
     @brief Creates a new task

     This creates a new ITask object and returns it. The backend shall fill out the task object and
     put it into the database using the insertTask() method. Afterwards, the backend shall destroy
     the object.
   */
  virtual ITask* createTask() = 0;

  /**
   * @brief Reads an account from the database
   *
   * This reads the account with the given @p uuid from the database. If no such account
   * exists, this returns a nullptr. The backend shall delete the returned object once it is done.
   */
  virtual IAccount* getAccount( const QUuid &uuid ) = 0;

  /**
   * @brief Reads a todo list from the database.
   *
   * This reads the todo list with the given @p uuid from the database. If no such todo list exists,
   * a nullptr is returned. The backend shall delete the returned object once it is done.
   */
  virtual ITodoList* getTodoList( const QUuid &uuid ) = 0;

  /**
   * @brief Reads a todo from the database
   *
   * This reads the todo with the given @p uuid from the database. If no such todo exists, a
   * nullptr is returned. The backend shall delete the returned object once it is done.
   */
  virtual ITodo* getTodo( const QUuid &uuid ) = 0;

  /**
   * @brief Reads a task from the database
   *
   * This reads the task with the given @p uuid from the database. If no such task exists, a nullptr
   * is returned. The backend shall delete the returned object once it is done.
   */
  virtual ITask* getTask( const QUuid &uuid ) = 0;

  /**
     @brief Gets accounts from the database

     This retrieves accounts from the database. The @p flags allow to filter the retrieved
     objects (e.g. whether to retrieve only modified or disposed objects). If @p maxAccounts
     is greater than 0, then at most maxAccounts accounts are returned, otherwise all matching
     accounts are returned. If @p offset is greater or equal to 0, then from all matching accounts,
     objects starting at the given offset are returned.

     @note The backend is responsible to delete the returned objects.
   */
  virtual QList<IAccount*> getAccounts(
      QueryFlags flags = QueryAny,
      int maxAccounts = 0,
      int offset = 0 ) = 0;

  /**
     @brief Gets todo lists from the database

     This retrieves todo lists from the database. The @p flags allow to filter the retrieved
     objects (e.g. whether to retrieve only modified or disposed objects). If @p maxTodoLists
     is greater than 0, then at most maxTodoLists todo lists are returned, otherwise all matching
     todo lists are returned. If @p offset is greater or equal to 0, then from all matching todo lists,
     objects starting at the given offset are returned.

     @note The backend is responsible to delete the returned objects.
   */
  virtual QList<ITodoList*> getTodoLists(
      QueryFlags flags = QueryAny,
      int maxTodoLists = 0,
      int offset = 0 ) = 0;

  /**
     @brief Gets todos from the database

     This retrieves todos from the database. The @p flags allow to filter the retrieved
     objects (e.g. whether to retrieve only modified or disposed objects). If @p maxTodos
     is greater than 0, then at most maxTodos todos are returned, otherwise all matching
     todos are returned. If @p offset is greater or equal to 0, then from all matching todos,
     objects starting at the given offset are returned.

     @note The backend is responsible to delete the returned objects.
   */
  virtual QList<ITodo*> getTodos(
      QueryFlags flags = QueryAny,
      int maxTodos = 0,
      int offset = 0 ) = 0;

  /**
     @brief Gets tasks from the database

     This retrieves tasks from the database. The @p flags allow to filter the retrieved
     objects (e.g. whether to retrieve only modified or disposed objects). If @p maxTasks
     is greater than 0, then at most maxTasks accounts are returned, otherwise all matching
     accounts are returned. If @p offset is greater or equal to 0, then from all matching tasks,
     objects starting at the given offset are returned.

     @note The backend is responsible to delete the returned objects.
   */
  virtual QList<ITask*> getTasks(
      QueryFlags flags = QueryAny,
      int maxTasks = 0,
      int offset = 0 ) = 0;

  /**
       @brief Insert or update an account

       This method is used to insert or update an @p account into the database. If there is no
       account with the same IAccount::uuid(), then the account will be added to the database
       as a new one. Otherwise, the existing account will be updated. Upon updating, any potential
       changes that were done in the frontend will be overridden. Furthermore, using this will
       unset both the @b dirty as well as the @b disposed flags.

       @sa createAccount()
       @sa getAccount()
     */
  virtual bool insertAccount( IAccount *account ) = 0;

  /**
       @brief Adds or updates a todo list

       This will either add the todo @p list to the database or update it (if
       a list with the same uuid is already in the database). Any changes done in the
       front end will be overridden by this. Furthermore, both the @b dirty as well
       as the @b disposed flag will be reset.

       @sa createTodoList()
       @sa getTodoList()
     */
  virtual bool insertTodoList( ITodoList *list ) = 0;

  /**
       @brief Adds or updates a todo

       This will either add the @p todo to the database or update it
       (if another record with the same uuid is already in the
       database). This will override any changes done in the front end. Furthermore,
       both the @b dirty as well as the @b disposed flags will be reset.

       @sa createTodo()
       @sa getTodo()
     */
  virtual bool insertTodo( ITodo *todo ) = 0;

  /**
       @brief Inserts or updates a @p task into the database

       This will insert the @p task into the database or update it, if a task with the appropriate
       uuid already exists. This will override any potential changes done in the frontend.
       Furthermore, both the @b dirty as well as the @b disposed flags will be reset.

       @sa createTask()
       @sa getTask()
     */
  virtual bool insertTask( ITask *task ) = 0;

  /**
     @brief Mark an account as saved

     This shall be called after the backend saved an modified @p account in its backing service.

     @sa getAccounts()
   */
  virtual bool onAccountSaved( IAccount *account ) = 0;

  /**
     @brief Mark a todo list as saved

     This shall be called for a modified @p todoList after saving it in the backend's
     backing service.

     @sa getTodoLists()
   */
  virtual bool onTodoListSaved( ITodoList *todoList ) = 0;

  /**
     @brief Mark a todo as saved

     This shall be called after a backend saved a modified @p todo in its backing service.

     @sa getTodos()
   */
  virtual bool onTodoSaved( ITodo *todo ) = 0;

  /**
     @brief Mark a task as saved

     A backend shall call this method after saving a modified @p task in its backing service.

     @sa getTasks()
   */
  virtual bool onTaskSaved( ITask *task ) = 0;

  /**
       @brief Deletes the @p account from the database

       This will remove the @p account and all todo lists belonging to it from the database.
     */
  virtual bool deleteAccount( IAccount *account ) = 0;

  /**
       @brief Removes a todo list from the database

       This will remove the todo @p list and all contained todos from the database.
     */
  virtual bool deleteTodoList( ITodoList *list ) = 0;

  /**
       @brief Removes a todo from the database

       This removes the @p todo and all contained tasks from the database.
     */
  virtual bool deleteTodo( ITodo *todo ) = 0;

  /**
       @brief Removes a @p task from the database

       This removed the @p task from the database.
     */
  virtual bool deleteTask( ITask *task ) = 0;

};

/**
   @brief An interface that is implemented by storage backends

   This interface must be implemented by plugins that implement access to
   a kind of todo list storage. Classes implementing this interface typically have to fulfill the
   following tasks:

   - Handling of accounts (see also IAccount)
   - Reading objects from a given account and storing it into the application database (see
     also IDatabase)
   - Finally, regularly checking the database for objects that changed and store them back into
     the appropriate account.
 */
class IBackend {

public:

  /**
     @brief The capabilities a backend supports

     This enum describes the various optional capabilities that a backend might support.
     A backend can use this in the capabilities() method to indicate to the application
     which operations are allowed with this backend. For example, a backend might want to
     omit the CanCreateAccount capability in case it supports only one locally created
     connection to some service (in the simplest case, when a backend just saves the data
     into a local directory in the file system). In such cases, the application might want to
     create one single IAccount and use this for all todo lists. Similarly, in such a use
     case a backend also wants to avoid a user deleting this account. So consequentially
     the backend would omit the CanDisposeAccount capability as well.

     @note Not all capabilities might get honoured in the front end! Defining a set of
           capabilities does not mean that the backend does not need to do sanity checks on
           the data it has to care for.
   */
  enum Capabilities {
    CanCreateAccount = 0,
    CanCreateTodoList,
    CanCreateTodo,
    CanCreateTask,
    CanDisposeAccount,
    CanDisposeTodoList,
    CanDisposeTodo,
    CanDisposeTask
  };

  /**
       @brief Destructor

       Required for some compilers.
     */
  virtual ~IBackend() {}

  /**
       @brief Sets the todo list database to be used by the backend

       This sets the @p database that shall be used by the backend. This method is called
       once when the application statrts and initializes the various plugins. The backend should
       store the database reference to be able to interact with the application later on.
     */
  virtual void setDatabase( IDatabase* database ) = 0;

  /**
       @brief Sets the directory for locally stored files

       Sets the @p directory where the backend should put local files into. The
       backend might use this directory for storing any persistent files into. Contents of this
       directory will remain there across application sessions.
     */
  virtual void setLocalStorageDirectory( const QString& directory ) = 0;

  /**
       @brief The identifier of the backend

       Returns a string that identifies the backend. Two different backends
       must use different names.

       @note This is used internally for the application logic. In the user interface, this string
             will never be visible. Note also that the value returned must not change between
             applications essions (otherwise, the database will get invalid).
     */
  virtual QString name() const = 0;

  /**
       @brief The (human readable) name of the backend

       Returns the name of the interface. This is used when presenting the
       backend in listings.
     */
  virtual QString title() const = 0;

  /**
       @brief Returns a description for the backend

       This method shall return a string that describes the storage backend. This text is shown when
       the user creates new accounts.
     */
  virtual QString description() const = 0;

  /**
     @brief The capabilities the backend supports.

     A backend has to implement this method and return the list of all supported capabilities.

     @sa Capabilities
   */
  virtual QSet<Capabilities> capabilities() const = 0;

  /**
       @brief Start the backend

       This method is called once when the application starts up. It is a signal to the backend that
       it shall initialize itself.

       @returns True when starting the backend was successfull or
                false otherwise.
     */
  virtual bool start() = 0;

  /**
       @brief Stop the backend

       This method is called once when the application terminates. It is a signal to the backend
       to finalize whatever it does (e.g. shut down of connections to external services or releasing
       of resources).

       @return True in case the backend was successfully stopped or false
               otherwise.
     */
  virtual bool stop() = 0;

  /**
     @brief Gives the backend a hint for synchronization

     Unless backends have other means for synchronizing their external data with the internal
     database, they shall implement any syncing in this method. The application will regularly call
     this method.

     If a backend does implement its synchronization solely on its own, it has to provide an
     empty implementation.

     @note It is adviced to use this method to do any expensive syncing instead of implementing
           own means. This is because the application might hook into the host operating system
           to provide ideal points in time for synchronization and hence save e.g. battery when
           running on mobile devices.
   */
  virtual void sync() = 0;

};

} /* namespace OpenTodoList */

Q_DECLARE_INTERFACE(OpenTodoList::IBackend, "net.rpdev.OpenTodoList.IBackend/1.0")


#endif // OPENTODOLISTINTERFACES_H

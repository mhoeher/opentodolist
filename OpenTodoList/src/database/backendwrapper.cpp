#include "backendwrapper.h"

#include "datamodel/todolistlibrary.h"
#include "datamodel/account.h"
#include "datamodel/todolist.h"
#include "datamodel/todo.h"
#include "datamodel/task.h"

#include "database/queries/insertaccount.h"
#include "database/queries/inserttodolist.h"
#include "database/queries/inserttodo.h"
#include "database/queries/inserttask.h"

#include <QDebug>

namespace OpenTodoList {

namespace DataBase {

BackendWrapper::BackendWrapper(QObject *parent) :
    QObject( parent ),
    m_database( 0 ),
    m_backend( 0 ),
    m_status( Invalid )
{
}

BackendWrapper::BackendWrapper(Database *database, IBackend *backend, QObject *parent) :
    QObject(parent),
    m_database( database ),
    m_backend( backend ),
    m_status( Stopped )
{
    Q_ASSERT( m_database );
    Q_ASSERT( m_backend );

    m_backend->setDatabase( this );
}

BackendWrapper::~BackendWrapper()
{
    // Don't do this! The actual backend is released when the plugin's
    // QObject instance is destroyed!
    //delete m_backend;
}

bool BackendWrapper::insertAccount(IAccount *account)
{
    DataModel::Account *acc = static_cast< DataModel::Account* >( account );
    Queries::InsertAccount q( acc, Queries::InsertAccount::NoFlags, StorageQuery::CallerIsOwner );
    m_database->runQuery( &q );
    return true;
}

bool BackendWrapper::insertTodoList(ITodoList *list)
{
    DataModel::TodoList *todoList = static_cast< DataModel::TodoList* >( list );
    Queries::InsertTodoList q( todoList );
    m_database->runQuery( &q );
    return true;
}

bool BackendWrapper::insertTodo(ITodo *todo)
{
    DataModel::Todo *t = static_cast< DataModel::Todo* >( todo );
    Queries::InsertTodo q( t );
    m_database->runQuery( &q );
    return true;
}

bool BackendWrapper::insertTask(ITask *task)
{
    DataModel::Task *t = static_cast< DataModel::Task* >( task );
    Queries::InsertTask q( t );
    m_database->runQuery( &q );
    return true;
}

bool BackendWrapper::deleteAccount(const IAccount *account)
{
    // TODO: Implement me
}

bool BackendWrapper::deleteTodoList(const ITodoList *list)
{
    // TODO: Implement me
}

bool BackendWrapper::deleteTodo(const ITodo *todo)
{
    // TODO: Implement me
}

bool BackendWrapper::deleteTask(const ITask *task)
{
    return false;
    // TODO: Implement me
}

IAccount *BackendWrapper::createAccount()
{
    // TODO: Check whether we want to use the DataBase object to be the one and only factory for this

    DataModel::Account *account = new DataModel::Account();
    account->setBackend( name() ); // inject our name
    return account;
}

ITodoList *BackendWrapper::createTodoList()
{
    // TODO: Check whether we want to use the DataBase object to be the one and only factory for this
    return new DataModel::TodoList();
}

ITodo *BackendWrapper::createTodo()
{
    // TODO: Check whether we want to use the DataBase object to be the one and only factory for this
    return new DataModel::Todo();
}

ITask *BackendWrapper::createTask()
{
    // TODO: Check whether we want to use the DataBase object to be the one and only factory for this
  return new DataModel::Task();
}

IAccount *BackendWrapper::getAccount(const QUuid &uuid)
{
  // TODO: Implement me
  return nullptr;
}

ITodoList *BackendWrapper::getTodoList(const QUuid &uuid)
{
  // TODO: Implement me
  return nullptr;
}

ITodo *BackendWrapper::getTodo(const QUuid &uuid)
{
  // TODO: Implement me
  return nullptr;
}

ITask *BackendWrapper::getTask(const QUuid &uuid)
{
  // TODO: Implement me
  return nullptr;
}

QList<IAccount *> BackendWrapper::getModifiedAccounts(int maxAccounts)
{
  // TODO: implement me
  return QList<IAccount*>();
}

QList<ITodoList *> BackendWrapper::getModifiedTodoLists(int maxTodoLists)
{
  //TODO: Implement me
  return QList<ITodoList*>();
}

QList<ITodo *> BackendWrapper::getModifiedTodos(int maxTodos)
{
  //TODO: Implement me
  return QList<ITodo*>();
}

QList<ITask *> BackendWrapper::getModifiedTasks(int maxTasks)
{
  //TODO: Implement me
  return QList<ITask*>();
}

bool BackendWrapper::onAccountSaved(IAccount *account)
{
  //TODO: Implement me
}

bool BackendWrapper::onTodoListSaved(ITodoList *todoList)
{
  //TODO: Implement me
}

bool BackendWrapper::onTodoSaved(ITodo *todo)
{
  //TODO: Implement me
}

bool BackendWrapper::onTaskSaved(ITask *task)
{
  //TODO: Implement me
}

QList<IAccount *> BackendWrapper::getDeletedAccounts(int maxAccounts)
{
  //TODO: Implement me
}

QList<ITodoList *> BackendWrapper::getDeletedTodoLists(int maxTodoLists)
{
  //TODO: Implement me
}

QList<ITodo *> BackendWrapper::getDeletedTodos(int maxTodos)
{
  //TODO: Implement me
}

QList<ITask *> BackendWrapper::getDeletedTasks(int maxTasks)
{
  //TODO: Implement me
}

void BackendWrapper::setLocalStorageDirectory(const QString &directory)
{
    if ( m_status != Invalid )
        m_backend->setLocalStorageDirectory( directory );
}

QString BackendWrapper::name() const
{
    if ( m_status != Invalid )
        return m_backend->name();
    return QString();
}

QString BackendWrapper::title() const
{
    if ( m_status != Invalid )
        return m_backend->title();
    return QString();
}

QString BackendWrapper::description() const
{
    if ( m_status != Invalid )
        return m_backend->description();
    return QString();
}

bool BackendWrapper::start()
{
    switch ( m_status ) {
    case Invalid: return false;
    case Running: return true;
    case Stopped:
        if ( m_backend->start() ) {
            setStatus( Running );
            return true;
        } else {
            return false;
        }
    }
    qWarning() << "Unhandled status in BackendWrapper::start():" << m_status;
    return false;
}

bool BackendWrapper::stop()
{
    switch ( m_status ) {
    case Invalid: return false;
    case Stopped: return true;
    case Running:
        if ( m_backend->stop() ) {
            setStatus( Stopped );
            return true;
        } else {
            return false;
        }
    }
    qWarning() << "Unhandled status in BackendWrapper::stop():" << m_status;
    return false;
}

void BackendWrapper::doStart()
{
    if ( !start() ) {
        qWarning() << "Failed to start backend" << title();
    } else {
        qDebug() << "Started backend" << title();
    }
}

void BackendWrapper::doStop()
{
    if ( !stop() ) {
        qWarning() << "Failed to stop backend" << title();
    } else {
        qDebug() << "Stopped backend" << title();
    }
}

void BackendWrapper::setDatabase(IDatabase *database)
{
    Q_ASSERT( false );
    Q_UNUSED( database );
}

void BackendWrapper::setStatus(BackendWrapper::Status newStatus)
{
    if ( m_status != newStatus ) {
        m_status = newStatus;
        emit statusChanged();
    }
}

} /* DataBase */

} /* OpenTodoList */

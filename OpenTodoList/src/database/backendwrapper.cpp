#include "backendwrapper.h"

#include "datamodel/todolistlibrary.h"

#include "datamodel/account.h"
#include "datamodel/todolist.h"
#include "datamodel/todo.h"

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

bool BackendWrapper::insertAccount(const IAccount *account)
{
    // TODO: Implement me
    return false;
}

bool BackendWrapper::insertTodoList(const ITodoList *list)
{
    return false;
    //return m_status != Invalid && m_database->insertTodoList( m_backend->id(), list );
}

bool BackendWrapper::insertTodo(const ITodo *todo)
{
    return false;
    //return m_status != Invalid && m_database->insertTodo( m_backend->id(), todo );
}

bool BackendWrapper::deleteAccount(const IAccount *account)
{
    // TODO: Implement me
}

bool BackendWrapper::deleteTodoList(const ITodoList *list)
{
    return m_status != Invalid && m_database->deleteTodoList( m_backend->id(), list );
}

bool BackendWrapper::deleteTodo(const ITodo *todo)
{
    return m_status != Invalid && m_database->deleteTodo( m_backend->id(), todo );
}

IAccount *BackendWrapper::createAccount()
{
    // TODO: Check whether we want to use the DataBase object to be the one and only factory for this
    return new DataModel::Account();
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

void BackendWrapper::setLocalStorageDirectory(const QString &directory)
{
    if ( m_status != Invalid )
        m_backend->setLocalStorageDirectory( directory );
}

QString BackendWrapper::id() const
{
    if ( m_status != Invalid )
        return m_backend->id();
    return QString();
}

QString BackendWrapper::name() const
{
    if ( m_status != Invalid )
        return m_backend->name();
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
        qWarning() << "Failed to start backend" << name();
    } else {
        qDebug() << "Started backend" << name();
    }
}

void BackendWrapper::doStop()
{
    if ( !stop() ) {
        qWarning() << "Failed to stop backend" << name();
    } else {
        qDebug() << "Stopped backend" << name();
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

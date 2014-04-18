#include "backendwrapper.h"

#include "todolistlibrary.h"

#include <QDebug>

BackendWrapper::BackendWrapper(TodoListLibrary* library, BackendInterface *backend, QObject *parent) :
    QObject(parent),
    m_library( library ),
    m_backend( backend ),
    m_status( Stopped )
{
    Q_ASSERT( m_library );
    Q_ASSERT( m_backend );

    m_backend->setDatabase( this );
}

BackendWrapper::~BackendWrapper()
{
    // Don't do this! The actual backend is released when the plugin's
    // QObject instance is destroyed!
    //delete m_backend;
}

bool BackendWrapper::insertTodoList(const TodoListStruct &list)
{
    return m_library->insertTodoList( m_backend, list );
}

bool BackendWrapper::insertTodo(const TodoStruct &todo)
{
    return m_library->insertTodo( m_backend, todo );
}

bool BackendWrapper::deleteTodoList(const TodoListStruct &list)
{
    return m_library->deleteTodoList( m_backend, list );
}

bool BackendWrapper::deleteTodo(const TodoStruct &todo)
{
    return m_library->deleteTodo( m_backend, todo );
}

TodoListStruct BackendWrapper::nullTodoList()
{
    TodoListStruct result;
    result.id = QUuid();
    result.meta = QVariantMap();
    result.name = QString();
    return result;
}

TodoStruct BackendWrapper::nullTodo()
{
    TodoStruct result;
    result.deleted = false;
    result.description = QString();
    result.dueDate = QDateTime();
    result.id = QUuid();
    result.meta = QVariantMap();
    result.parentTodoId = QUuid();
    result.priority = -1;
    result.progress = 0;
    result.title = QString();
    result.todoListId = QUuid();
    result.weight = 0.0;
    return result;
}

void BackendWrapper::setLocalStorageDirectory(const QString &directory)
{
    m_backend->setLocalStorageDirectory( directory );
}

QString BackendWrapper::id() const
{
    return m_backend->id();
}

QString BackendWrapper::name() const
{
    return m_backend->name();
}

QString BackendWrapper::description() const
{
    return m_backend->description();
}

bool BackendWrapper::start()
{
    switch ( m_status ) {
    case Running: return true;
    case Stopped:
        if ( m_backend->start() ) {
            m_status = Running;
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
    case Stopped: return true;
    case Running:
        if ( m_backend->stop() ) {
            m_status = Stopped;
            return true;
        } else {
            return false;
        }
    }
    qWarning() << "Unhandled status in BackendWrapper::stop():" << m_status;
    return false;
}

bool BackendWrapper::notifyTodoListChanged(const TodoListStruct &list)
{
    return m_backend->notifyTodoListChanged( list );
}

bool BackendWrapper::notifyTodoChanged(const TodoStruct &todo)
{
    return m_backend->notifyTodoChanged( todo );
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

void BackendWrapper::setDatabase(TodoListDatabase *database)
{
    Q_ASSERT( false );
    Q_UNUSED( database );
}

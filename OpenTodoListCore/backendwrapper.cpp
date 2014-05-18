#include "backendwrapper.h"

#include "todolistlibrary.h"

#include <QDebug>

namespace BackendWrapperFuncs {

TodoListStruct nullTodoList()
{
    TodoListStruct result;
    result.id = QUuid();
    result.meta = QVariantMap();
    result.name = QString();
    return result;
}

TodoStruct nullTodo()
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

}

const TodoListStruct BackendWrapper::NullTodoList = BackendWrapperFuncs::nullTodoList();
const TodoStruct BackendWrapper::NullTodo = BackendWrapperFuncs::nullTodo();

BackendWrapper::BackendWrapper(QObject *parent) :
    QObject( parent ),
    m_library( 0 ),
    m_backend( 0 ),
    m_status( Invalid )
{

}

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
    return m_status != Invalid && m_library->insertTodoList( m_backend, list );
}

bool BackendWrapper::insertTodo(const TodoStruct &todo)
{
    return m_status != Invalid && m_library->insertTodo( m_backend, todo );
}

bool BackendWrapper::deleteTodoList(const TodoListStruct &list)
{
    return m_status != Invalid && m_library->deleteTodoList( m_backend, list );
}

bool BackendWrapper::deleteTodo(const TodoStruct &todo)
{
    return m_status != Invalid && m_library->deleteTodo( m_backend, todo );
}

TodoListStruct BackendWrapper::nullTodoList()
{
    return NullTodoList;
}

TodoStruct BackendWrapper::nullTodo()
{
    return NullTodo;
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

bool BackendWrapper::notifyTodoListChanged(const TodoListStruct &list)
{
    return m_status != Invalid && m_backend->notifyTodoListChanged( list );
}

bool BackendWrapper::notifyTodoChanged(const TodoStruct &todo)
{
    return m_status != Invalid && m_backend->notifyTodoChanged( todo );
}

void BackendWrapper::notifyTodoListDeleted(const TodoListStruct &list)
{
    if ( m_status != Invalid ) {
        m_backend->notifyTodoListDeleted( list );
    }
}

void BackendWrapper::notifyTodoDeleted(const TodoStruct &todo)
{
    if ( m_status != Invalid ) {
        m_backend->notifyTodoDeleted( todo );
    }
}

bool BackendWrapper::canAddTodo(const TodoListStruct &list, const TodoStruct &todo)
{
    return m_status != Invalid && m_backend->canAddTodo( list, todo );
}

void BackendWrapper::addTodo( TodoStruct newTodo, const TodoListStruct &list, const TodoStruct &todo)
{
    if ( m_status != Invalid )
        m_backend->addTodo( newTodo, list, todo );
}

bool BackendWrapper::canAddTodoList()
{
    return m_status != Invalid && m_backend->canAddTodoList();
}

void BackendWrapper::addTodoList(TodoListStruct newList)
{
    if ( m_status != Invalid )
        m_backend->addTodoList( newList );
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

void BackendWrapper::setStatus(BackendWrapper::Status newStatus)
{
    if ( m_status != newStatus ) {
        m_status = newStatus;
        emit statusChanged();
    }
}

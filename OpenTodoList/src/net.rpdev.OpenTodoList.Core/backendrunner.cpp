#include "backendrunner.h"

#include "todolistlibrary.h"

BackendRunner::BackendRunner(TodoListLibrary *library, QObject *parent) :
    QObject(parent),
    m_thread(),
    m_backends()
{
    m_backends.reserve( library->plugins()->backendInterfaces().size() );
    m_thread.start();
    foreach ( BackendInterface *interface, library->plugins()->backendInterfaces() ) {
        BackendWrapper *wrapper = new BackendWrapper( library, interface );
        wrapper->setLocalStorageDirectory( localStorageLocation( wrapper->id() ) );
        wrapper->moveToThread( &m_thread );
        connect( this, SIGNAL(startBackends()), wrapper, SLOT(doStart()) );
        connect( this, SIGNAL(stopBackends()), wrapper, SLOT(doStop()), Qt::BlockingQueuedConnection );
        m_backends << wrapper;
    }
}

BackendRunner::~BackendRunner()
{
    m_thread.quit();
    m_thread.wait();
    foreach ( BackendWrapper* wrapper, m_backends ) {
        delete wrapper;
    }
}

/**
   @brief Check whether new todos can be added

   Returns true of a new todo can be created within the given @p list and using the specified
   parent @p todo in the @p backend. If the parent todo is an invalid todo structure, this returns
   whether adding new top level todos is supported.
 */
bool BackendRunner::canAddTodo(const QString &backend, const TodoListStruct &list, const TodoStruct &todo)
{
    BackendWrapper *b = backendByName( backend );
    if ( b ) {
        return b->canAddTodo( list, todo );
    }
    return false;
}

/**
   @brief Requests creation of a todo

   Requests createn of the @p newTodo in the given todo @p list in the @p backend.
   If set to a valid structure, the todo is created as a sub-todo of the parent @p todo.
 */
void BackendRunner::addTodo(const QString &backend, TodoStruct &newTodo,
                            const TodoListStruct &list, const TodoStruct &todo)
{
    BackendWrapper *b = backendByName( backend );
    if ( b ) {
        QMetaObject::invokeMethod( b, "addTodo", Qt::QueuedConnection,
                                   Q_ARG( TodoStruct, newTodo ),
                                   Q_ARG( TodoListStruct, list ),
                                   Q_ARG( TodoStruct, todo ) );
    }
}

/**
   @brief Check if a new todo list can be created

   Check if we can create todo lists in the @p backend. If so, return true or false otherwise.
 */
bool BackendRunner::canAddTodoList(const QString &backend)
{
    BackendWrapper *b = backendByName( backend );
    if ( b ) {
        return b->canAddTodoList();
    }
    return false;
}

/**
   @brief Create a new todo list

   Requests creation of a @p newList in the @p backend.
 */
void BackendRunner::addTodoList(const QString &backend, TodoListStruct newList)
{
    BackendWrapper *b = backendByName( backend );
    if ( b ) {
        QMetaObject::invokeMethod( b, "addTodoList", Qt::QueuedConnection,
                                   Q_ARG( TodoListStruct, newList ) );
    }
}

/**
   @brief The backends maintained by the runner
 */
QVector<BackendWrapper*> BackendRunner::backends() const
{
    return m_backends;
}

void BackendRunner::start()
{
    emit startBackends();
}

void BackendRunner::stop()
{
    emit stopBackends();
}

void BackendRunner::notifyTodoListChanged(const QString &backend, const TodoListStruct &list)
{
    BackendWrapper *b = backendByName( backend );
    if ( b ) {
        QMetaObject::invokeMethod(
                    b, "notifyTodoListChanged", Qt::QueuedConnection, Q_ARG(TodoListStruct, list ) );
    }
}

void BackendRunner::notifyTodoChanged(const QString &backend, const TodoStruct &todo)
{
    BackendWrapper *b = backendByName( backend );
    if ( b ) {
        QMetaObject::invokeMethod(
                    b, "notifyTodoChanged", Qt::QueuedConnection, Q_ARG(TodoStruct, todo ) );
    }
}

void BackendRunner::notifyTodoListDeleted(const QString &backend, const TodoListStruct &list)
{
    BackendWrapper *b = backendByName( backend );
    if ( b ) {
        QMetaObject::invokeMethod(
                    b, "notifyTodoListDeleted", Qt::QueuedConnection, Q_ARG(TodoListStruct, list ) );
    }
}

void BackendRunner::notifyTodoDeleted(const QString &backend, const TodoStruct &todo)
{
    BackendWrapper *b = backendByName( backend );
    if ( b ) {
        QMetaObject::invokeMethod(
                    b, "notifyTodoDeleted", Qt::QueuedConnection, Q_ARG(TodoStruct, todo ) );
    }
}

BackendWrapper *BackendRunner::backendByName(const QString &backend) const
{
    for ( int i = 0; i < m_backends.size(); ++i ) {
        if ( m_backends.at( i )->id() == backend ) {
            return m_backends.at( i );
        }
    }
    return 0;
}

#ifdef Q_OS_ANDROID
/**
   @brief Returns the external data location on Android

   This returns a directory where data can be put on Android that shall
   remain intact even when the application is uninstalled and installed later
   on.

   @todo Hardcoded package name - this is not portable.

   @return The external storage location on Android
 */
QString BackendRunner::androidExtStorageLocation()
{
    QString s( qgetenv( "EXTERNAL_STORAGE" ) );
    QDir dir( s + "/data/net.rpdev.opentodolist/" );
    return dir.absolutePath();
}
#endif

/**
   @brief Returns the directory where a given backend should write to

   This returns a directory that shall be used by the backend of the given
   @p type to write data into.

   @return A location where todo lists for the given @p type can be stored or an empty string
           if no fitting location could be found.
 */
QString BackendRunner::localStorageLocation(const QString &type)
{
    QStringList locations = QStandardPaths::standardLocations( QStandardPaths::DataLocation );

#ifdef Q_OS_ANDROID
    locations.insert( 0, androidExtStorageLocation() );
#endif

    foreach ( QString location, locations ) {
        QDir dir( location + "/" + type );
        if ( !dir.exists() ) {
            dir.mkpath( dir.absolutePath() );
        }
        if ( dir.exists() ) {
            return dir.path();
        }
    }
    return QString();
}

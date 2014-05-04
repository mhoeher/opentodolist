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

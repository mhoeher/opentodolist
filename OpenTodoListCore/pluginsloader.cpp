#include "pluginsloader.h"

#include <QCoreApplication>
#include <QDir>
#include <QPluginLoader>

#include <QtDebug>

PluginsLoader::PluginsLoader(QObject *parent) :
    QObject(parent),
    m_backends( new Backends( this ) )
{
    foreach ( QString libraryPath, QCoreApplication::libraryPaths() ) {
        // Load backends
        QDir dir( libraryPath + "/opentodobackends" );
        foreach ( QString entry, dir.entryList( QDir::Files ) ) {
            QString pluginPath = dir.absolutePath() + "/" + entry;
            QPluginLoader* loader = new QPluginLoader( pluginPath, this );
            if ( loader->load() ) {
                QObject* instance = loader->instance();
                OpenTodoListBackend *backend =
                        qobject_cast< OpenTodoListBackend* >( instance );
                if ( backend ) {
                    
                    m_backends->append( backend );
                }
            }
        }
    }
}

PluginsLoader::Backends* PluginsLoader::backends() const
{
    return m_backends;
}

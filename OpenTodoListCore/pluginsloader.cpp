#include "pluginsloader.h"

#include <QCoreApplication>
#include <QDir>
#include <QPluginLoader>

PluginsLoader::PluginsLoader(QObject *parent) :
    QObject(parent)
{
    foreach ( QString libraryPath, QCoreApplication::libraryPaths() ) {
        // Load backends
        QDir dir( libraryPath + "/opentodobackends" );
        foreach ( QString entry, dir.entryList() ) {
            QString pluginPath = libraryPath + "/" + entry;
            QPluginLoader* loader = new QPluginLoader( pluginPath, this );
            if ( loader->load() ) {
                QObject* instance = loader->instance();
                OpenTodoListBackend *backend =
                        qobject_cast< OpenTodoListBackend* >( instance );
                if ( backend ) {
                    m_backends << backend;
                }
            }
        }
    }
}

const QList<OpenTodoListBackend *> &PluginsLoader::backends() const
{
    return m_backends;
}


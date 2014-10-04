/*
 *  OpenTodoList - A todo and task manager
 *  Copyright (C) 2013, 2014  Martin Höher <martin@rpdev.net>
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

#ifndef PLUGINSLOADER_H
#define PLUGINSLOADER_H

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QPluginLoader>
#include <QObject>
#include <QVector>

/**
  @brief Generic class that can be used to load plugins
 */
template<typename InterfaceType>
class PluginsLoader : public QObject
{
public:

    explicit PluginsLoader(const QString pluginTypeName, QObject *parent);

    /**
       @brief Returns the list of loaded plugins.
     */
    QVector<InterfaceType*> plugins() const {
        return m_plugins;
    }

private:
    QVector<InterfaceType*> m_plugins;

    void addPlugin( QObject *plugin );

};

/**
   @brief Constructor

   Creates the plugins loader. This will search for plugins in the registered
   library paths (see QCoreApplication::libraryPaths) in a subdirectory called
   @p pluginTypeName. For each loaded plugin, it is checked whether it exports
   a plugin instance which implements the @p InterfaceType.
 */
template<typename InterfaceType>
PluginsLoader<InterfaceType>::PluginsLoader(const QString pluginTypeName, QObject *parent) :
    QObject( parent ) {
    // load static instances
    foreach ( QObject* instance, QPluginLoader::staticInstances() ) {
        //instance->setParent( this ); // static instances already have parent!
        addPlugin( instance );
    }

    foreach ( QString libraryPath, QCoreApplication::libraryPaths() ) {

        QStringList entries;
        // Load backends
#ifdef Q_OS_ANDROID
        QDir dir( libraryPath );
        entries = dir.entryList(
                    QStringList() << QString("*net_rpdev_opentodolist_plugins_%1*").arg( pluginTypeName ),
                    QDir::Files );
#else
        QDir dir( libraryPath + "/" + pluginTypeName );
        entries = dir.entryList( QDir::Files );
#endif

        foreach ( QString entry, entries ) {
            QString pluginPath = dir.absolutePath() + "/" + entry;
            QPluginLoader* loader = new QPluginLoader( pluginPath, this );
            if ( loader->load() ) {
                loader->instance()->setParent( this );
                addPlugin( loader->instance() );
            } else {
                qDebug() << "Failed to load" << loader->fileName() << "because of:" << loader->errorString();
            }
        }
    }
}

template<typename InterfaceType>
void PluginsLoader<InterfaceType>::addPlugin(QObject *plugin)
{
    InterfaceType *instance = qobject_cast< InterfaceType* >( plugin );
    if ( instance ) {
        m_plugins.append( instance );
    }
}

#endif // PLUGINSLOADER_H

/*
 *  OpenTodoList - A todo and task manager
 *  Copyright (C) 2013  Martin Höher <martin@rpdev.net>
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

#include "todolistfactory.h"

#include <QDir>
#include <QStandardPaths>

TodoListFactory::TodoListFactory(const QString& type, QObject* parent) :
    QObject(parent),
    m_type( type )
{
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
QString TodoListFactory::androidExtStorageLocation()
{
    QString s( qgetenv( "EXTERNAL_STORAGE" ) );
    QDir dir( s+ "/data/net.rpdev.opentodolist/" );
    return dir.absolutePath();
}
#endif

/**
   @brief Returns a local directory where the todo lists of a specific type can be saved

   This will return a path where todo list data of lists of a certain @p type
   can be stored. The path includes the type specifier of the factory
   so each factory has a dedicated location where to store data. The returned
   path is guaranteed to be writable and it exists unless an empty string is
   returned.

   @return A location where todo lists for the given @p type can be stored or an empty string
           if no fitting location could be found.
 */
QString TodoListFactory::localStorageLocation(const QString &type)
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

/**
   @brief Returns a local directory where the todo list data can be stored

   This will return a path where todo list data of lists created by this
   factory can be stored. The path includes the type specifier of the factory
   so each factory has a dedicated location where to store data. The returned
   path is guaranteed to be writable and it exists unless an empty string is
   returned.

   @return A location where todo lists for this can be stored or an empty string
           if no fitting location could be found.
 */
QString TodoListFactory::localStorageLocation() const
{
    return localStorageLocation( m_type );
}

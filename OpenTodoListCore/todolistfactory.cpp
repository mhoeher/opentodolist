/*
 *  OpenTodoListCore - Core functionality for the OpenTodoList application
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
    QStringList locations = QStandardPaths::standardLocations( QStandardPaths::DataLocation );
    foreach ( QString location, locations ) {
        QDir dir( location + "/" + m_type );
        if ( !dir.exists() ) {
            dir.mkpath( dir.absolutePath() );
        }
        if ( dir.exists() ) {
            return dir.path();
        }
    }
    return QString();
}

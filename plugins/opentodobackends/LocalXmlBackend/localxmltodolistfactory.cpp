/*
 *  LocalXmlBackend - Local XML storage for OpenTodoList
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


#include "localxmltodolistfactory.h"

#include "localxmltodolist.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QStandardPaths>

LocalXmlTodoListFactory::LocalXmlTodoListFactory(const QString& type, QObject* parent) : 
    TodoListFactory( type, parent)
{

}

AbstractTodoList* LocalXmlTodoListFactory::createTodoList(QObject* parent, const QString& key ) const
{
    QString lkey = key;
    if ( lkey.isNull() ) {
        QStringList locations = QStandardPaths::standardLocations( QStandardPaths::ConfigLocation );
        QString baseDir;
        foreach ( QString location, locations ) {
            QDir dir( location + "/" + QCoreApplication::organizationName() + "/OpenTodoList/LocalXmlBackend" );
            if ( !dir.exists() ) {
                dir.mkpath( dir.absolutePath() );
            }
            if ( dir.exists() ) {
                baseDir = dir.absolutePath();
                break;
            }
        }
        int i = 0;
        while ( QFile::exists( ( lkey = QString( "%1/%2" ).arg( baseDir ).arg( i ) ) ) ) {
            ++i;
        }
    }
    return new LocalXmlTodoList( lkey, type(), parent );
}


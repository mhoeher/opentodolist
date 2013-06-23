/*
 *  OpenTodoListDesktopQml - Desktop QML frontend for OpenTodoList
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

#include <QtGui/QGuiApplication>
#include "applicationviewer.h"

#include "todolistlibrary.h"

#include <QQmlEngine>
#include <QQmlContext>
#include <QRegExp>

int main(int argc, char *argv[])
{
    
    QCoreApplication::setApplicationName( "OpenTodoList" );
    QCoreApplication::setApplicationVersion( "0.0.0" );
    QCoreApplication::setOrganizationDomain( "www.rpdev.net" );
    QCoreApplication::setOrganizationName( "RPdev" );
    
    QGuiApplication app(argc, argv);
    
    QString basePath = QCoreApplication::applicationDirPath() + "/qml/OpenTodoListDesktopQml";
    
    foreach ( QString arg, app.arguments() ) {
        static const QRegExp basePathRE( "^--base-path=(.*)$" );
        if ( basePathRE.indexIn( arg ) == 0 ) {
            basePath = basePathRE.cap( 1 );   
        }
    }
    
    QCoreApplication::addLibraryPath( QCoreApplication::applicationDirPath() + "/plugins" );
    
    TodoListLibrary* library = new TodoListLibrary( &app );

    // Prepare an object holding various "settings" to be passed into QML
    QObject* settings = new QObject( &app );
    // Use "debug" mode in QML (used occasionally to make development in "debug" mode easier)
#ifdef QT_DEBUG
    settings->setProperty( "debug", true );
#else
    settings->setProperty( "debug", false );
#endif
    
    ApplicationViewer viewer;
    viewer.addImportPath( basePath );
    viewer.engine()->rootContext()->setContextProperty( QStringLiteral("library"), library );
    viewer.engine()->rootContext()->setContextProperty( QStringLiteral("settings"), settings );
    viewer.setMainFile(basePath + "/main.qml");
    viewer.showExpanded();
    
    return app.exec();
}

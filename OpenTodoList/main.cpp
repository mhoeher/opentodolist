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

#include <QtGui/QGuiApplication>
#include "applicationviewer.h"

#include "todolistlibrary.h"
#include "todosortfiltermodel.h"

#include "settings.h"

#include <QQmlEngine>
#include <QQmlContext>
#include <QRegExp>
#include <QString>
#include <QtQml>
#include <QVariantMap>

#include <iostream>

// FIXME: Remove this as soon as we can load plugins dynamically
#ifdef Q_OS_ANDROID
#include <QtPlugin>
Q_IMPORT_PLUGIN(LocalXmlBackend)
#endif

int main(int argc, char *argv[])
{   
    QCoreApplication::setApplicationName( "OpenTodoList" );
    QCoreApplication::setApplicationVersion( "0.0.0" );
    QCoreApplication::setOrganizationDomain( "www.rpdev.net" );
    QCoreApplication::setOrganizationName( "RPdev" );
    
    QGuiApplication app(argc, argv);
    
    QString basePath = ":/qml/OpenTodoList";
    bool showHelp = false;

    foreach ( QString arg, app.arguments() ) {

        static const QRegExp basePathRE( "^--base-path=(.*)$" );
        static const QRegExp helpRE( "^(--help|-h|/h|/\\?)$" );

        if ( basePathRE.indexIn( arg ) == 0 ) {
            basePath = basePathRE.cap( 1 );   
        } else if ( helpRE.indexIn( arg ) == 0 ) {
            showHelp = true;
        }
    }

    if ( showHelp ) {
        std::cout
             << "OpenTodoList - A todo and task manager" << std::endl
             << "Copyright (C) 2013  RPdev" << std::endl
             << std::endl
             << "Usage:" << std::endl
             << "  " << QCoreApplication::applicationFilePath().toStdString() << " [OPTIONS]" << std::endl
             << std::endl
             << "    --base-path=PATH Sets the search path for resource files." << std::endl
             << "                     This is for development purpose." << std::endl
             << "    --help -h /h /?  Shows this help" << std::endl
             << std::endl
             << "For further information, please check the project website at:" << std::endl
             << "  http://rpdev.net/home/project/opentodolist" << std::endl
             << std::endl;
        return 0;
    }
    
    QCoreApplication::addLibraryPath( QCoreApplication::applicationDirPath() + "/plugins" );
#ifdef Q_OS_ANDROID
    QCoreApplication::addLibraryPath( "assets:/plugins" );
#endif
    
    TodoListLibrary* library = new TodoListLibrary( &app );

    qmlRegisterType<TodoSortFilterModel>("net.rpdev.OpenTodoList", 1, 0, "TodoSortFilterModel");
    qmlRegisterType<Settings>("net.rpdev.OpenTodoList", 1, 0, "Settings" );
    
    ApplicationViewer viewer( basePath );

    viewer.addImportPath( basePath );
    viewer.rootContext()->setContextProperty( "library", library );
    viewer.rootContext()->setContextProperty( "applicationViewer", &viewer );
    viewer.loadSettings();

    viewer.show();
    int result = app.exec();
    viewer.saveSettings();
    return result;
}

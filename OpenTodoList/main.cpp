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
#include <QtQml>
#include <QVariantMap>

#include <iostream>

#ifdef Q_OS_ANDROID
Q_IMPORT_PLUGIN(LocalXmlBackend)
#endif

QVariantMap getOsMap() {
    QVariantMap result;
#ifdef Q_OS_AIX
    result.insert( "AIX", true );
#endif
#ifdef Q_OS_ANDROID
    result.insert( "ANDROID", true );
#endif
#ifdef Q_OS_BSD4
    result.insert( "BSD4", true );
#endif
#ifdef Q_OS_BSDI
    result.insert( "BSDI", true );
#endif
#ifdef Q_OS_CYGWIN
    result.insert( "CYGWIN", true );
#endif
#ifdef Q_OS_DARWIN
    result.insert( "DARWIN", true );
#endif
#ifdef Q_OS_DGUX
    result.insert( "DGUX", true );
#endif
#ifdef Q_OS_DYNIX
    result.insert( "DYNIX", true );
#endif
#ifdef Q_OS_FREEBSD
    result.insert( "FREEBSD", true );
#endif
#ifdef Q_OS_HPUX
    result.insert( "HPUX", true );
#endif
#ifdef Q_OS_HURD
    result.insert( "HURD", true );
#endif
#ifdef Q_OS_IOS
    result.insert( "IOS", true );
#endif
#ifdef Q_OS_IRIX
    result.insert( "IRIX", true );
#endif
#ifdef Q_OS_LINUX
    result.insert( "LINUX", true );
#endif
#ifdef Q_OS_LYNX
    result.insert( "LYNX", true );
#endif
#ifdef Q_OS_MAC
    result.insert( "MAC", true );
#endif
#ifdef Q_OS_MACX
    result.insert( "MACX", true );
#endif
#ifdef Q_OS_NETBSD
    result.insert( "NETBSD", true );
#endif
#ifdef Q_OS_OPENBSD
    result.insert( "OPENBSD", true );
#endif
#ifdef Q_OS_OSF
    result.insert( "OSF", true );
#endif
#ifdef Q_OS_QNX
    result.insert( "QNX", true );
#endif
#ifdef Q_OS_RELIANT
    result.insert( "RELIANT", true );
#endif
#ifdef Q_OS_SCO
    result.insert( "SCO", true );
#endif
#ifdef Q_OS_SOLARIS
    result.insert( "SOLARIS", true );
#endif
#ifdef Q_OS_ULTRIX
    result.insert( "ULTRIX", true );
#endif
#ifdef Q_OS_UNIX
    result.insert( "UNIX", true );
#endif
#ifdef Q_OS_UNIXWARE
    result.insert( "UNIXWARE", true );
#endif
#ifdef Q_OS_WIN32
    result.insert( "WIN32", true );
#endif
#ifdef Q_OS_WIN64
    result.insert( "WIN64", true );
#endif
#ifdef Q_OS_WIN
    result.insert( "WIN", true );
#endif
#ifdef Q_OS_WINCE
    result.insert( "WINCE", true );
#endif
    return result;
}

int main(int argc, char *argv[])
{   
    QCoreApplication::setApplicationName( "OpenTodoList" );
    QCoreApplication::setApplicationVersion( "0.0.0" );
    QCoreApplication::setOrganizationDomain( "www.rpdev.net" );
    QCoreApplication::setOrganizationName( "RPdev" );
    
    QGuiApplication app(argc, argv);
    
    QString basePath = ":/qml/OpenTodoList";
    QString style;
    bool showHelp = false;

    foreach ( QString arg, app.arguments() ) {

        static const QRegExp basePathRE( "^--base-path=(.*)$" );
        static const QRegExp styleRE( "^--style=(.*)$" );
        static const QRegExp helpRE( "^(--help|-h|/h|/\\?)$" );

        if ( basePathRE.indexIn( arg ) == 0 ) {
            basePath = basePathRE.cap( 1 );   
        } else if ( styleRE.indexIn( arg ) == 0 ) {
            style = styleRE.cap( 1 );
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
             << "    --style=STYLE    Sets the style to use. Possible values are:" << std::endl
             << "                     widget - Use Widget style (for typical Desktop environments)" << std::endl
             << "                     neutral - Use a platform/OS agnostic touch enabled style." << std::endl
             << "    --help -h /h /?  Shows this help" << std::endl
             << std::endl
             << "For further information, please check the project website at:" << std::endl
             << "  http://rpdev.net/home/project/opentodolist" << std::endl
             << std::endl;
        return 0;
    }
    
    QCoreApplication::addLibraryPath( QCoreApplication::applicationDirPath() + "/plugins" );
    
    TodoListLibrary* library = new TodoListLibrary( &app );

    // Prepare an object holding various "settings" to be passed into QML
    QObject* settingsObject = new QObject( &app );

    // Use "debug" mode in QML (used occasionally to make development in "debug" mode easier)
#ifdef QT_DEBUG
    settingsObject->setProperty( "debug", true );
#else
    settingsObject->setProperty( "debug", false );
#endif

    qmlRegisterType<TodoSortFilterModel>("net.rpdev.OpenTodoList", 1, 0, "TodoSortFilterModel");
    qmlRegisterType<Settings>("net.rpdev.OpenTodoList", 1, 0, "Settings" );
    
    ApplicationViewer viewer( basePath + "/styles/" );

    viewer.addImportPath( basePath );
    viewer.rootContext()->setContextProperty( QStringLiteral("library"), library );
    viewer.rootContext()->setContextProperty( QStringLiteral("settings"), settingsObject );
    viewer.rootContext()->setContextProperty( QStringLiteral("os"), getOsMap() );
    viewer.rootContext()->setContextProperty( QStringLiteral("applicationViewer"), &viewer );
    viewer.loadSettings();
    if ( !style.isEmpty() && style != viewer.currentStyle() ) {
        viewer.setCurrentStyle( style );
    }

    int result = app.exec();
    viewer.saveSettings();
    return result;
}

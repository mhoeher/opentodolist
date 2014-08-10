#include <QDir>
#include <QtGui/QGuiApplication>
#include <QQmlEngine>
#include "qtquick2applicationviewer.h"

#include <iostream>

#include <QDirIterator>
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication::setApplicationName( "OpenTodoList" );
    QCoreApplication::setApplicationVersion( "0.0.0" );
    QCoreApplication::setOrganizationDomain( "www.rpdev.net" );
    QCoreApplication::setOrganizationName( "RPdev" );

    QGuiApplication app(argc, argv);

    if ( app.arguments().contains( "-version" ) ||
         app.arguments().contains( "--version" ) ) {
        std::cout << "OpenTodoList version " << VERSION << std::endl;
        return 0;
    }


    QtQuick2ApplicationViewer viewer;

    // Add plugin search paths for QML plugins
#ifdef Q_OS_ANDROID
    QCoreApplication::addLibraryPath( QCoreApplication::applicationDirPath() );
    viewer.engine()->addPluginPath( QCoreApplication::applicationDirPath() );
#else
    QDirIterator di( app.applicationDirPath(), QDirIterator::Subdirectories );
    while ( di.hasNext() ) {
        qDebug() << di.next();
    }

    QString basePath = app.applicationDirPath();
    QDir baseDir( basePath );
    if ( baseDir.cdUp() ) {
        if ( baseDir.cd( "plugins" ) ) {
            QCoreApplication::addLibraryPath( baseDir.absolutePath() );
            foreach ( QString entry, baseDir.entryList( QDir::Dirs ) ) {
                viewer.engine()->addPluginPath( baseDir.absoluteFilePath( entry ) );
            }
        }
    }
#endif

#ifdef Q_OS_ANDROID
    viewer.addImportPath( QStringLiteral("qrc:/OpenTodoList/qml"));
    viewer.addImportPath( QStringLiteral("assets:/OpenTodoList/qml"));
    viewer.setMainQmlFile("qrc:/OpenTodoList/qml/OpenTodoList/main.qml");
#else
    viewer.addImportPath( basePath + "/../share/OpenTodoList/qml" );
    viewer.setMainQmlFile( basePath + "/../share/OpenTodoList/qml/OpenTodoList/main.qml" );
#endif
    viewer.showExpanded();

    return app.exec();
}

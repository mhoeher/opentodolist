#include <QDir>
#ifdef HAS_QT_WIDGETS
#include <QApplication>
#else
#include <QtGui/QGuiApplication>
#endif
#include <QQmlEngine>
#include <QQmlContext>
#include "applicationinstance.h"
#include "commandhandler.h"
#include "qtquick2applicationviewer.h"
#include "statusnotifiericon.h"

#include "opentodolistcoreqmlplugin.h"
#include "opentodolistwidgetsqmlplugin.h"

#include <iostream>

#include <QDirIterator>
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication::setApplicationName( "OpenTodoList" );
    QCoreApplication::setApplicationVersion( "0.0.0" );
    QCoreApplication::setOrganizationDomain( "www.rpdev.net" );
    QCoreApplication::setOrganizationName( "RPdev" );

#ifdef HAS_QT_WIDGETS
    QApplication app(argc, argv);
#else
    QGuiApplication app(argc, argv);
#endif

#ifndef Q_OS_ANDROID
    app.setQuitOnLastWindowClosed( false );
#endif

    if ( app.arguments().contains( "-version" ) ||
         app.arguments().contains( "--version" ) ) {
        std::cout << "OpenTodoList version " << VERSION << std::endl;
        return 0;
    }

    ApplicationInstance instance( QCoreApplication::applicationName() );
    if ( instance.state() == ApplicationInstance::InstanceIsSecondary ) {
        qDebug() << "Running secondary instance. Contacting server and quitting...";
        instance.sendMessage( CommandHandler::show() );
        return 0;
    }

    // Register "Core" plugin:
    OpenTodoListCoreQmlPlugin corePlugin;
    corePlugin.registerTypes( "net.rpdev.OpenTodoList.Core" );

    // Register "Widgets" plugin:
    OpenTodoListWidgetsQmlPlugin widgetsPlugin;
    widgetsPlugin.registerTypes( "net.rpdev.OpenTodoList.Widgets" );

    QtQuick2ApplicationViewer viewer;

    CommandHandler commandHandler;
    commandHandler.setApplicationWindow( &viewer );
    QObject::connect( &instance, SIGNAL(receivedMessage(QString)),
                      &commandHandler, SLOT(handleMessage(QString)) );

    // Add plugin search paths for QML plugins
#ifdef Q_OS_ANDROID
    QCoreApplication::addLibraryPath( QCoreApplication::applicationDirPath() );
    viewer.engine()->addPluginPath( QCoreApplication::applicationDirPath() );
#else
    QString basePath = app.applicationDirPath();
#ifdef Q_OS_MACX
    QString pluginsDirName = "PlugIns";
#else
    QString pluginsDirName = "plugins";
#endif
    QDir baseDir( basePath );
    if ( baseDir.cdUp() ) {
        if ( baseDir.cd( pluginsDirName ) ) {
            QCoreApplication::addLibraryPath( baseDir.absolutePath() );
            foreach ( QString entry, baseDir.entryList( QDir::Dirs ) ) {
                viewer.engine()->addPluginPath( baseDir.absoluteFilePath( entry ) );
            }
        }
    }
#endif

#if defined(Q_OS_UNIX) && !defined(Q_OS_MACX)
    app.setWindowIcon( QIcon( basePath + "/../share/OpenTodoList/icons/OpenTodoList80.png" ) );
#endif

    StatusNotifierIcon statusNotifier( &commandHandler );
    statusNotifier.setApplicationTitle( app.applicationName() );
#ifdef Q_OS_ANDROID
    statusNotifier.setApplicationIcon( QIcon( "qrc:/icons/OpenTodoList80.png" ) );
#else
    statusNotifier.setApplicationIcon(
                QIcon( basePath + "/../share/OpenTodoList/icons/OpenTodoList80.png" ) );
#endif
    statusNotifier.show();

    viewer.engine()->rootContext()->setContextProperty(
                QStringLiteral( "statusNotifier" ), &statusNotifier );
    viewer.engine()->rootContext()->setContextProperty(
                QStringLiteral( "commandHandler" ), &commandHandler );


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

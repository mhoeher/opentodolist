#include <QDir>
#ifdef HAS_QT_WIDGETS
#include <QApplication>
#else
#include <QtGui/QGuiApplication>
#endif
#include <QQmlEngine>
#include <QQmlContext>
#include "qtquick2applicationviewer.h"

#include "systemintegration/applicationinstance.h"
#include "systemintegration/commandhandler.h"
#include "systemintegration/statusnotifiericon.h"

#include "core/coreplugin.h"
#include "datamodel/datamodelplugin.h"
#include "database/databaseplugin.h"
#include "models/modelsplugin.h"
#include "systemintegration/systemintegrationplugin.h"

#include <iostream>

#include <QDirIterator>
#include <QDebug>

using namespace OpenTodoList;

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

    SystemIntegration::ApplicationInstance instance( QCoreApplication::applicationName() );
    if ( instance.state() == SystemIntegration::ApplicationInstance::InstanceIsSecondary ) {
        qDebug() << "Running secondary instance. Contacting server and quitting...";
        instance.sendMessage( SystemIntegration::CommandHandler::show() );
        return 0;
    }

    // Register "Core" plugin:
    OpenTodoList::Core::Plugin corePlugin;
    corePlugin.registerTypes( "net.rpdev.OpenTodoList.Core" );

    // Register "DataModel" plugin:
    OpenTodoList::DataModel::Plugin dataModelPlugin;
    dataModelPlugin.registerTypes( "net.rpdev.OpenTodoList.DataModel" );

    // Register "DataBase" plugin:
    OpenTodoList::DataBase::Plugin dataBasePlugin;
    dataBasePlugin.registerTypes( "net.rpdev.OpenTodoList.DataBase" );

    // Register "Models" plugin:
    OpenTodoList::Models::Plugin modelsPlugin;
    modelsPlugin.registerTypes( "net.rpdev.OpenTodoList.Models" );

    // Register "SystemIntegration" plugin:
    OpenTodoList::SystemIntegration::Plugin systemIntegrationPlugin;
    systemIntegrationPlugin.registerTypes( "net.rpdev.OpenTodoList.SystemIntegration" );

    QtQuick2ApplicationViewer viewer;

    SystemIntegration::CommandHandler commandHandler;
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

#if defined(Q_OS_UNIX) && !defined(Q_OS_MACX) && !defined(Q_OS_ANDROID)
    app.setWindowIcon( QIcon( basePath + "/../share/OpenTodoList/icons/OpenTodoList80.png" ) );
#endif

    SystemIntegration::StatusNotifierIcon statusNotifier( &commandHandler );
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
    viewer.engine()->rootContext()->setContextProperty( QStringLiteral( "commandHandler" ), &commandHandler );

    viewer.addImportPath( QStringLiteral("qrc:/qml") );
    viewer.setMainQmlFile("qrc:/qml/OpenTodoList/main.qml");
    viewer.showExpanded();

    return app.exec();
}

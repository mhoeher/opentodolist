#include <QDir>
#include <QQmlEngine>
#include <QQmlContext>

#include <iostream>

#include "systemintegration/application.h"

using namespace OpenTodoList;

int startService( int &argc, char *argv[] ) {
    QCoreApplication::setApplicationName( "OpenTodoList" );
    QCoreApplication::setApplicationVersion( VERSION );
    QCoreApplication::setOrganizationDomain( "www.rpdev.net" );
    QCoreApplication::setOrganizationName( "RPdev" );

    SystemIntegration::Application *app = new SystemIntegration::Application( argc, argv );

    if ( app->arguments().contains( "-version" ) ||
         app->arguments().contains( "--version" ) ) {
        std::cout << "OpenTodoList version " << VERSION << std::endl;
        return 0;
    }

    if ( app->instance()->state() ==
         OpenTodoList::SystemIntegration::ApplicationInstance::InstanceIsSecondary ) {
        return 1;
    }
    return 0;
}

int stopService() {
    if ( qApp ) {
        delete qApp;
        return 0;
    }
    return 1;
}

int startApp() {
    SystemIntegration::Application *app = qobject_cast< SystemIntegration::Application* >( qApp );
    if ( app ) {
        app->showWindow();
        return app->exec();
    }
    return 1;
}

int main(int argc, char *argv[])
{
    if ( startService( argc, argv ) == 0 ) {
        int exitCode = startApp();
        stopService();
        return exitCode;
    }
    return 1;
}

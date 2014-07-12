#include <QtGui/QGuiApplication>
#include "qtquick2applicationviewer.h"

#include <QtPlugin>

#include <iostream>

///////////////////////////////////////////////////////////////////////////////
// Import plugins:
///////////////////////////////////////////////////////////////////////////////

// backends:
Q_IMPORT_PLUGIN(LocalXmlBackend)

// QML:
Q_IMPORT_PLUGIN(OpenTodoListCoreQmlPlugin)


int main(int argc, char *argv[])
{
    // Initialize resources from static libraries
    Q_INIT_RESOURCE(OpenTodoListCore);

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
    viewer.addImportPath(QStringLiteral("qrc:/OpenTodoList/imports"));
#ifdef Q_OS_ANDROID
    viewer.addImportPath( QStringLiteral("qrc:/OpenTodoList/qml"));
    viewer.setMainQmlFile("qrc:/OpenTodoList/qml/OpenTodoList/main.qml");
#else
    QString basePath = app.applicationDirPath();
    viewer.addImportPath( basePath + "/../share/OpenTodoList/qml" );
    viewer.setMainQmlFile( basePath + "/../share/OpenTodoList/qml/OpenTodoList/main.qml" );
#endif
    viewer.showExpanded();

    return app.exec();
}

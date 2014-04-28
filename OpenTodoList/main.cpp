#include <QtGui/QGuiApplication>
#include "qtquick2applicationviewer.h"

#include <QtPlugin>

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

    QtQuick2ApplicationViewer viewer;
    viewer.addImportPath(QStringLiteral("qrc:/OpenTodoList/imports"));
    viewer.addImportPath( QStringLiteral("qrc:/OpenTodoList/qml"));
    viewer.setMainQmlFile("qrc:/OpenTodoList/qml/OpenTodoList/main.qml");
    viewer.showExpanded();

    return app.exec();
}

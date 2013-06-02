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
    
    QString basePath = QStringLiteral("../share/OpenTodoList/qml/OpenTodoListDesktopQml");
    
    foreach ( QString arg, app.arguments() ) {
        static const QRegExp basePathRE( "^--base-path=(.*)$" );
        if ( basePathRE.indexIn( arg ) == 0 ) {
            basePath = basePathRE.cap( 1 );   
        }
    }
    
    QCoreApplication::addLibraryPath( "../lib/opentodolist/plugins" );
    
    TodoListLibrary* library = new TodoListLibrary( &app );
    
    ApplicationViewer viewer;
    viewer.addImportPath( basePath );
    viewer.engine()->rootContext()->setContextProperty( QStringLiteral("library"), library );
    viewer.setMainFile(basePath + "/main.qml");
    viewer.showExpanded();
    
    return app.exec();
}

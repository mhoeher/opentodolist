#include "systemintegration/systemintegrationplugin.h"

#include "application.h"
#include "shortcut.h"

#include <qqml.h>
#include <QJSEngine>

namespace OpenTodoList {

/**
 * @brief Classes integrating the application into the host system
 *
 * This namespace contains various classes that are used to integrate the application into
 * the host operating system and desktop environment.
 */
namespace SystemIntegration {

Plugin::Plugin(QObject *parent) :
    QQmlExtensionPlugin(parent)
{
}

void Plugin::registerTypes(const char *uri)
{
    //@uri net.rpdev.OpenTodoList.SystemIntegration
    qmlRegisterType< Shortcut >( uri, 1, 0,  "Shortcut" );
    qmlRegisterUncreatableType< CommandHandler >( uri, 1, 0, "CommandHandler", "Use Application.handler instead!" );
    qmlRegisterUncreatableType< ApplicationInstance >( uri, 1, 0, "ApplicationInstance", "Use Application.instance instead!" );
    qmlRegisterUncreatableType< StatusNotifierIcon >( uri, 1, 0, "StatusNotifierIcon", "Use Application.notifierIcon instead!" );
    qmlRegisterUncreatableType< Application >( uri, 1, 0, "Application", "Use application global variable instead!" );
}

} /* SystemIntegration */

} /* OpenTodoList */

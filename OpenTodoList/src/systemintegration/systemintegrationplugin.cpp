#include "systemintegration/systemintegrationplugin.h"

#include "shortcut.h"

#include <qqml.h>

namespace OpenTodoList {

namespace SystemIntegration {

Plugin::Plugin(QObject *parent) :
    QQmlExtensionPlugin(parent)
{
}

void Plugin::registerTypes(const char *uri)
{
    //@uri net.rpdev.OpenTodoList.SystemIntegration
    qmlRegisterType< Shortcut >( uri, 1, 0,  "Shortcut" );
}

} /* SystemIntegration */

} /* OpenTodoList */

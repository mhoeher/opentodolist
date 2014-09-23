#include "opentodolistwidgetsqmlplugin.h"

#include "shortcut.h"

#include <qqml.h>


OpenTodoListWidgetsQmlPlugin::OpenTodoListWidgetsQmlPlugin(QObject *parent) :
    QQmlExtensionPlugin(parent)
{
}

void OpenTodoListWidgetsQmlPlugin::registerTypes(const char *uri)
{
    //@uri net.rpdev.OpenTodoList.Widgets
    qmlRegisterType< Shortcut >( uri, 1, 0,  "Shortcut" );
}

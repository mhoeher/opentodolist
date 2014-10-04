#include "database/databaseplugin.h"

#include "database/backendwrapper.h"

#include <qqml.h>
#include <QJSEngine>

namespace OpenTodoList {

namespace DataBase {

Plugin::Plugin(QObject *parent) :
    QQmlExtensionPlugin(parent)
{
}

void Plugin::registerTypes(const char *uri)
{
    //@uri net.rpdev.OpenTodoList.DataBase
    qmlRegisterType< BackendWrapper >(
                uri, 1, 0, "BackendWrapper" );
}

} /* DataBase */

} /* OpenTodoList */

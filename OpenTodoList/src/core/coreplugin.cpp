#include "core/coreplugin.h"

#include "core/documentformatter.h"
#include "core/settings.h"

#include <qqml.h>
#include <QJSEngine>

namespace OpenTodoList {

namespace Core {

Plugin::Plugin(QObject *parent) :
    QQmlExtensionPlugin(parent)
{
}

void Plugin::registerTypes(const char *uri)
{
    //@uri net.rpdev.OpenTodoList.Core
    qmlRegisterType<Settings>( uri, 1, 0, "Settings" );
    qmlRegisterType<DocumentFormatter>( uri, 1, 0, "DocumentFormatter" );
}

} /* Core */

} /* OpenTodoList */

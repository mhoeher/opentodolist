#include "models/modelsplugin.h"

#include "models/todolistmodel.h"
#include "models/todomodel.h"

#include <qqml.h>
#include <QJSEngine>

namespace OpenTodoList {

namespace Models {

Plugin::Plugin(QObject *parent) :
    QQmlExtensionPlugin(parent)
{
}

void Plugin::registerTypes(const char *uri)
{
    //@uri net.rpdev.OpenTodoList.Models
    qmlRegisterType< TodoListModel >( uri, 1, 0, "TodoListModel" );
    qmlRegisterType< TodoModel >( uri, 1, 0, "TodoModel" );
}

} /* Models */

} /* OpenTodoList */

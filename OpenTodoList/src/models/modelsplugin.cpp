#include "models/modelsplugin.h"

#include "models/accountmodel.h"
#include "models/backendmodel.h"
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
    qmlRegisterType< AccountModel >( uri, 1, 0, "AccountModel" );
    qmlRegisterType< BackendModel >( uri, 1, 0, "BackendModel" );
    qmlRegisterType< TodoListModel >( uri, 1, 0, "TodoListModel" );
    qmlRegisterType< TodoModel >( uri, 1, 0, "TodoModel" );
}

} /* Models */

} /* OpenTodoList */

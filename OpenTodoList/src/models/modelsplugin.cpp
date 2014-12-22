#include "models/modelsplugin.h"

#include "models/accountmodel.h"
#include "models/backendmodel.h"
#include "models/todolistmodel.h"
#include "models/todomodel.h"
#include "models/taskmodel.h"

#include <qqml.h>
#include <QJSEngine>

namespace OpenTodoList {

/**
 * @brief Models (implementing QAbstractItemModel) bridging into the application model
 *
 * This namespace provides several classes that make the objects defines in the
 * OpenTodoList::DataModel and stored in the OpenTodoList::DataBase available
 * in the application via a well defined interface described by QAbstractItemModel.
 */
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
    qmlRegisterType< TaskModel >( uri, 1, 0, "TaskModel" );
}

} /* Models */

} /* OpenTodoList */

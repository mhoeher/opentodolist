#include "database/databaseplugin.h"

#include "database/database.h"
#include "database/backendwrapper.h"
#include "database/databaseconnection.h"

#include <qqml.h>
#include <QJSEngine>

/**
 * @namespace OpenTodoList::DataBase::Queries
 * @brief Database queries
 *
 * This namespace is holding various queries that are used to modify or read the
 * application database.
 */
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
    qmlRegisterUncreatableType< Database >( uri, 1, 0, "Database", "Use application.database!" );
    qmlRegisterType< DatabaseConnection >( uri, 1, 0, "DatabaseConnection" );
}

} /* DataBase */

} /* OpenTodoList */

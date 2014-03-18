#include "opentodolistcoreqmlplugin.h"

#include "settings.h"
#include "todo.h"
#include "todolist.h"
#include "todolistlibrary.h"
#include "todosortfiltermodel.h"

#include <qqml.h>
#include <QJSEngine>

// static constructor for TodoListLibrary
static QJSValue net_rpdev_opentodolist_core_TodoListLibrary(
        QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    return scriptEngine->newQObject( new TodoListLibrary() );
}





OpenTodoListCoreQmlPlugin::OpenTodoListCoreQmlPlugin(QObject *parent) :
    QQmlExtensionPlugin(parent)
{
}

void OpenTodoListCoreQmlPlugin::registerTypes(const char *uri)
{
    //@uri net.rpdev.OpenTodoList.Core
    qmlRegisterUncreatableType< Todo >(
                uri, 1, 0, "Todo",
                "Todos should be created via factory method of TodoList" );
    qmlRegisterUncreatableType< TodoList >(
                uri, 1, 0, "TodoList",
                "TodoList should be created via factory method in TodoListLibrary" );

    qmlRegisterType<TodoSortFilterModel>( uri, 1, 0, "TodoSortFilterModel" );
    qmlRegisterType<Settings>( uri, 1, 0, "Settings" );
    qmlRegisterSingletonType( uri, 1, 0, "TodoListLibrary",
                              net_rpdev_opentodolist_core_TodoListLibrary );
}

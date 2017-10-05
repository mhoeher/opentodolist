#include "opentodolistqmlextensionsplugin.h"

#include "application.h"
#include "complexitem.h"
#include "documentformatter.h"
#include "image.h"
#include "item.h"
#include "library.h"
#include "note.h"
#include "task.h"
#include "todo.h"
#include "todolist.h"
#include "toplevelitem.h"
#include "itemcontainer.h"
#include "itemsmodel.h"
#include "itemssortfiltermodel.h"

#include "sync/synchronizer.h"
#include "sync/webdavsynchronizer.h"

#include "utils/updateservice.h"

#include <QtQml>

OpenTodoListQmlExtensionsPlugin::OpenTodoListQmlExtensionsPlugin(QObject *parent) :
  QQmlExtensionPlugin(parent)
{

}

void OpenTodoListQmlExtensionsPlugin::registerTypes(const char *uri)
{
  //@uri OpenTodoList

  qmlRegisterSingletonType<Application>(uri, 1, 0, "App",
                                        OpenTodoListQmlExtensionsPlugin::createApplication);
  qmlRegisterType<ComplexItem>(uri, 1, 0, "ComplexItem");
  qmlRegisterType<Image>(uri, 1, 0, "ImageTopLevelItem");
  qmlRegisterType<Item>(uri, 1, 0, "BasicItem");
  qmlRegisterType<DocumentFormatter>(uri, 1, 0, "DocumentFormatter");
  qmlRegisterType<Library>(uri, 1, 0, "Library");
  qmlRegisterType<Note>(uri, 1, 0, "Note");
  qmlRegisterType<Task>(uri, 1, 0, "Task");
  qmlRegisterType<Todo>(uri, 1, 0, "Todo");
  qmlRegisterType<TodoList>(uri, 1, 0, "TodoList");
  qmlRegisterType<TopLevelItem>(uri, 1, 0, "TopLevelItem");
  qmlRegisterType<ItemContainer>(uri, 1, 0, "ItemContainer");
  qmlRegisterType<ItemsModel>(uri, 1, 0, "ItemsModel");
  qmlRegisterType<ItemsSortFilterModel>(uri, 1, 0, "ItemsSortFilterModel");

  qmlRegisterUncreatableType<Synchronizer>(uri, 1, 0, "Synchronizer",
                                           "Use specific synchronizer");
  qmlRegisterType<WebDAVSynchronizer>(uri, 1, 0, "WebDAVSynchronizer");

  // To allow assigning model classes to the sourceModel
  // property of a proxy model in declarative assignments:
  qmlRegisterUncreatableType<QAbstractItemModel>(
              uri, 1, 0, "AbstractItemModel",
              "Use sub-classes of abstract item model instead");

  qmlRegisterType<UpdateService>(uri, 1, 0, "UpdateService");
}

QObject *OpenTodoListQmlExtensionsPlugin::createApplication(QQmlEngine *engine, QJSEngine *jsEngine)
{
  Q_UNUSED(engine);
  Q_UNUSED(jsEngine);
  return new Application();
}

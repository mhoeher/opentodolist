#include "opentodolistqmlextensionsplugin.h"

#include "application.h"
#include "datamodel/complexitem.h"
#include "datamodel/image.h"
#include "datamodel/item.h"
#include "datamodel/library.h"
#include "datamodel/note.h"
#include "datamodel/task.h"
#include "datamodel/todo.h"
#include "datamodel/todolist.h"
#include "datamodel/toplevelitem.h"
#include "datastorage/cache.h"
#include "models/itemsmodel.h"
#include "models/librariesmodel.h"
#include "models/itemssortfiltermodel.h"

#include "sync/synchronizer.h"
#include "sync/webdavsynchronizer.h"

#include "utils/updateservice.h"
#include "utils/syntaxhighlighter.h"

#ifdef Q_OS_ANDROID
#include "utils/android/androidfiledialog.h"
#endif

#include <QtQml>

OpenTodoListQmlExtensionsPlugin::OpenTodoListQmlExtensionsPlugin(QObject *parent) :
  QQmlExtensionPlugin(parent)
{

}

void OpenTodoListQmlExtensionsPlugin::registerTypes(const char *uri)
{
  //@uri OpenTodoList

  qmlRegisterSingletonType<Application>(
              uri, 1, 0, "Application",
              OpenTodoListQmlExtensionsPlugin::createApplication);
  qmlRegisterType<ComplexItem>(uri, 1, 0, "ComplexItem");
  qmlRegisterType<Image>(uri, 1, 0, "Image");
  qmlRegisterType<Cache>(uri, 1, 0, "Cache");
  qmlRegisterType<Item>(uri, 1, 0, "Item");
  qmlRegisterType<Library>(uri, 1, 0, "Library");
  qmlRegisterType<Note>(uri, 1, 0, "Note");
  qmlRegisterType<Task>(uri, 1, 0, "Task");
  qmlRegisterType<Todo>(uri, 1, 0, "Todo");
  qmlRegisterType<TodoList>(uri, 1, 0, "TodoList");
  qmlRegisterType<TopLevelItem>(uri, 1, 0, "TopLevelItem");
  qmlRegisterType<ItemsModel>(uri, 1, 0, "ItemsModel");
  qmlRegisterType<LibrariesModel>(uri, 1, 0, "LibrariesModel");
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
  qmlRegisterType<SyntaxHighlighter>(uri, 1, 0, "SyntaxHighlighter");

#ifdef Q_OS_ANDROID
  qmlRegisterType<AndroidFileDialog>(uri, 1, 0, "AndroidFileDialog");
#endif
}

QObject *OpenTodoListQmlExtensionsPlugin::createApplication(QQmlEngine *engine, QJSEngine *jsEngine)
{
  Q_UNUSED(engine);
  Q_UNUSED(jsEngine);
  return new Application();
}

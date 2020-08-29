/*
 * Copyright 2020 Martin Hoeher <martin@rpdev.net>
 +
 * This file is part of OpenTodoList.
 *
 * OpenTodoList is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * OpenTodoList is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenTodoList.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "opentodolistqmlextensionsplugin.h"

#include "application.h"
#include "datamodel/complexitem.h"
#include "datamodel/image.h"
#include "datamodel/item.h"
#include "datamodel/library.h"
#include "datamodel/note.h"
#include "datamodel/notepage.h"
#include "datamodel/task.h"
#include "datamodel/todo.h"
#include "datamodel/todolist.h"
#include "datamodel/toplevelitem.h"
#include "datastorage/cache.h"
#include "models/itemsmodel.h"
#include "models/librariesmodel.h"
#include "models/itemssortfiltermodel.h"

#include "sync/account.h"
#include "sync/nextcloudloginflow.h"
#include "sync/synchronizer.h"
#include "sync/webdavsynchronizer.h"

#include "utils/updateservice.h"
#include "utils/syntaxhighlighting.h"
#include "utils/problem.h"
#include "utils/problemmanager.h"

#ifdef Q_OS_ANDROID
#    include "utils/android/androidfiledialog.h"
#endif

#include <QtQml>

OpenTodoListQmlExtensionsPlugin::OpenTodoListQmlExtensionsPlugin(QObject* parent)
    : QQmlExtensionPlugin(parent)
{
}

void OpenTodoListQmlExtensionsPlugin::registerTypes(const char* uri)
{
    //@uri OpenTodoList

    qmlRegisterType<Account>(uri, 1, 0, "Account");
    qmlRegisterSingletonType<Application>(uri, 1, 0, "Application",
                                          OpenTodoListQmlExtensionsPlugin::createApplication);
    qmlRegisterType<ComplexItem>(uri, 1, 0, "ComplexItem");
    qmlRegisterType<Image>(uri, 1, 0, "Image");
    qmlRegisterType<Cache>(uri, 1, 0, "Cache");
    qmlRegisterType<Item>(uri, 1, 0, "Item");
    qmlRegisterType<Library>(uri, 1, 0, "Library");
    qmlRegisterType<Note>(uri, 1, 0, "Note");
    qmlRegisterType<NotePage>(uri, 1, 0, "NotePage");
    qmlRegisterType<Task>(uri, 1, 0, "Task");
    qmlRegisterType<Todo>(uri, 1, 0, "Todo");
    qmlRegisterType<TodoList>(uri, 1, 0, "TodoList");
    qmlRegisterType<TopLevelItem>(uri, 1, 0, "TopLevelItem");
    qmlRegisterType<ItemsModel>(uri, 1, 0, "ItemsModel");
    qmlRegisterType<LibrariesModel>(uri, 1, 0, "LibrariesModel");
    qmlRegisterType<ItemsSortFilterModel>(uri, 1, 0, "ItemsSortFilterModel");
    qmlRegisterType<ProblemManager>(uri, 1, 0, "ProblemManager");

    qmlRegisterUncreatableType<Synchronizer>(uri, 1, 0, "Synchronizer",
                                             "Use specific synchronizer");
    qmlRegisterType<WebDAVSynchronizer>(uri, 1, 0, "WebDAVSynchronizer");
    qmlRegisterType<NextCloudLoginFlow>(uri, 1, 0, "NextCloudLoginFlow");

    // To allow assigning model classes to the sourceModel
    // property of a proxy model in declarative assignments:
    qmlRegisterUncreatableType<QAbstractItemModel>(
            uri, 1, 0, "AbstractItemModel", "Use sub-classes of abstract item model instead");

    qmlRegisterType<UpdateService>(uri, 1, 0, "UpdateService");
    qmlRegisterType<SyntaxHighlighter>(uri, 1, 0, "SyntaxHighlighter");

#ifdef Q_OS_ANDROID
    qmlRegisterType<AndroidFileDialog>(uri, 1, 0, "AndroidFileDialog");
#endif
}

QObject* OpenTodoListQmlExtensionsPlugin::createApplication(QQmlEngine* engine, QJSEngine* jsEngine)
{
    Q_UNUSED(engine);
    Q_UNUSED(jsEngine);
    return new Application();
}

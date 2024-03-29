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
#include "sync/webdavaccount.h"
#include "sync/nextcloudaccount.h"
#include "sync/owncloudaccount.h"
#include "sync/synchronizer.h"
#include "sync/webdavsynchronizer.h"
#include "sync/dropboxaccount.h"

#include "utils/updateservice.h"
#include "utils/syntaxhighlighting.h"
#include "utils/problem.h"
#include "utils/problemmanager.h"
#include "utils/colors.h"
#include "utils/textutils.h"
#include "utils/fontutils.h"
#include "utils/shareutils.h"
#include "utils/fileeditobserver.h"

#include "SynqClient/NextCloudLoginFlow"

#ifdef Q_OS_ANDROID
#    include "utils/android/androidfiledialog.h"
#endif

#include <QtQml>

OpenTodoListQmlExtensionsPlugin::OpenTodoListQmlExtensionsPlugin(QObject* parent)
    : QQmlExtensionPlugin(parent), m_application(nullptr)
{
}

void OpenTodoListQmlExtensionsPlugin::registerTypes(const char* uri)
{
    //@uri OpenTodoList

    qmlRegisterUncreatableType<Account>(uri, 1, 0, "Account",
                                        "Use one of the concrete account types instead.");
    qmlRegisterType<WebDAVAccount>(uri, 1, 0, "WebDAVAccount");
    qmlRegisterType<NextCloudAccount>(uri, 1, 0, "NextCloudAccount");
    qmlRegisterType<OwnCloudAccount>(uri, 1, 0, "OwnCloudAccount");
    qmlRegisterType<DropboxAccount>(uri, 1, 0, "DropboxAccount");
    qmlRegisterSingletonType<Application>(uri, 1, 0, "Application",
                                          [=](QQmlEngine*, QJSEngine*) { return m_application; });
    qmlRegisterSingletonType<Colors>(uri, 1, 0, "Colors",
                                     [=](QQmlEngine*, QJSEngine*) { return new Colors(); });
    qmlRegisterSingletonInstance<Application>(uri, 1, 0, "Application", m_application);
    qmlRegisterType<ComplexItem>(uri, 1, 0, "ComplexItem");
    qmlRegisterType<Image>(uri, 1, 0, "ImageItem");
    qmlRegisterUncreatableType<Cache>(uri, 1, 0, "Cache", "Use Application.cache");
    qmlRegisterType<Item>(uri, 1, 0, "LibraryItem");
    qmlRegisterType<Library>(uri, 1, 0, "Library");
    qmlRegisterType<Note>(uri, 1, 0, "Note");
    qmlRegisterType<NotePage>(uri, 1, 0, "NotePageItem");
    qmlRegisterType<Task>(uri, 1, 0, "Task");
    qmlRegisterType<Todo>(uri, 1, 0, "Todo");
    qmlRegisterType<TodoList>(uri, 1, 0, "TodoList");
    qmlRegisterType<TopLevelItem>(uri, 1, 0, "TopLevelItem");
    qmlRegisterType<ItemsModel>(uri, 1, 0, "ItemsModel");
    qmlRegisterType<LibrariesModel>(uri, 1, 0, "LibrariesModel");
    qmlRegisterType<ItemsSortFilterModel>(uri, 1, 0, "ItemsSortFilterModel");
    qmlRegisterUncreatableType<ProblemManager>(uri, 1, 0, "ProblemManager",
                                               "Use Application.problemManager");
    qmlRegisterUncreatableType<Problem>(uri, 1, 0, "Problem", "Use ProblemManager to get Problems");

    qmlRegisterUncreatableType<Synchronizer>(uri, 1, 0, "Synchronizer",
                                             "Use specific synchronizer");
    qmlRegisterType<WebDAVSynchronizer>(uri, 1, 0, "WebDAVSynchronizer");
    qmlRegisterUncreatableType<SynqClient::NextCloudLoginFlow>(
            uri, 1, 0, "NextCloudLoginFlow",
            "Use OTL.Application.createNextCloudLoginFlow() instead");

    // To allow assigning model classes to the sourceModel
    // property of a proxy model in declarative assignments:
    qmlRegisterUncreatableType<QAbstractItemModel>(
            uri, 1, 0, "AbstractItemModel", "Use sub-classes of abstract item model instead");

    qmlRegisterType<UpdateService>(uri, 1, 0, "UpdateService");
    qmlRegisterType<SyntaxHighlighter>(uri, 1, 0, "SyntaxHighlighter");
    qmlRegisterType<TextUtils>(uri, 1, 0, "TextUtils");
    qmlRegisterType<FontUtils>(uri, 1, 0, "FontUtils");
    qmlRegisterType<ShareUtils>(uri, 1, 0, "ShareUtils");
    qmlRegisterType<FileEditObserver>(uri, 1, 0, "FileEditObserver");

#ifdef Q_OS_ANDROID
    qmlRegisterType<AndroidFileDialog>(uri, 1, 0, "AndroidFileDialog");
#endif
}

Application* OpenTodoListQmlExtensionsPlugin::application() const
{
    return m_application;
}

void OpenTodoListQmlExtensionsPlugin::setApplication(Application* application)
{
    m_application = application;
}

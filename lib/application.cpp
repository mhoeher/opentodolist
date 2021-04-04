/*
 * Copyright 2020-2021 Martin Hoeher <martin@rpdev.net>
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

#include "application.h"

#include <QClipboard>
#include <QCoreApplication>
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QGuiApplication>
#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QProcess>
#include <QRemoteObjectNode>
#include <QScopedPointer>
#include <QStandardPaths>
#include <QTextDocument>
#include <QTimer>
#include <QUuid>

#ifdef Q_OS_ANDROID
#    include <QtAndroid>
#    include <QAndroidJniExceptionCleaner>
#endif

#include "SynqClient/NextCloudLoginFlow"

#include "datamodel/image.h"
#include "datamodel/note.h"
#include "datamodel/notepage.h"
#include "datamodel/task.h"
#include "datamodel/todo.h"
#include "datamodel/todolist.h"
#include "datastorage/applicationsettings.h"
#include "datastorage/cache.h"
#include "datastorage/deleteitemsquery.h"
#include "datastorage/getitemquery.h"
#include "datastorage/insertorupdateitemsquery.h"
#include "datastorage/librariesitemsquery.h"
#include "datastorage/libraryloader.h"
#include "datastorage/movetodoquery.h"
#include "datastorage/promotetaskquery.h"
#include "fileutils.h"
#include "rep_backgroundservice_replica.h"
#include "sync/account.h"
#include "sync/synchronizer.h"
#include "sync/syncjob.h"
#include "sync/syncrunner.h"
#include "sync/webdavsynchronizer.h"
#include "utils/directorywatcher.h"
#include "utils/jsonutils.h"
#include "utils/keystore.h"

static Q_LOGGING_CATEGORY(log, "OpenTodoList.Application", QtDebugMsg);

/**
 * @brief Constructor.
 *
 * Creates a new Application object. The instance will be a child of the given
 * @p parent.
 */
Application::Application(Cache* cache, QObject* parent)
    : QObject(parent),
      m_cache(cache),
      m_keyStore(new KeyStore(this)),
      m_problemManager(new ProblemManager(this)),
      m_appSettings(new ApplicationSettings(m_cache, m_keyStore, m_problemManager, this)),
      m_settings(new QSettings(QSettings::IniFormat, QSettings::UserScope,
                               QCoreApplication::organizationName(),
                               QCoreApplication::applicationName(), this)),
      m_directoriesWithRunningSync(),
      m_syncErrors(),
      m_syncProgress(),
      m_librariesWithChanges(),
      m_remoteObjectNode(new QRemoteObjectNode(this)),
      m_backgroundService(),
      m_librariesRequestedForDeletion(),
      m_appInstanceUid(QUuid::createUuid()),
      m_propagateCacheEventsFromBackgroundService(false)
{
    Q_CHECK_PTR(cache);
    initialize();
}

/**
 * @brief Constructor.
 *
 * This is an overloaded version of the Application constructor. It causes the
 * Application object to store its settings in the given @p applicationDir.
 */
Application::Application(Cache* cache, const QString& applicationDir, QObject* parent)
    : QObject(parent),
      m_cache(cache),
      m_keyStore(new KeyStore(this)),
      m_problemManager(new ProblemManager(this)),
      m_appSettings(
              new ApplicationSettings(applicationDir, m_cache, m_keyStore, m_problemManager, this)),
      m_settings(new QSettings(applicationDir + "/appsettings.ini", QSettings::IniFormat, this)),
      m_directoriesWithRunningSync(),
      m_syncErrors(),
      m_syncProgress(),
      m_librariesWithChanges(),
      m_remoteObjectNode(new QRemoteObjectNode(this)),
      m_backgroundService(),
      m_librariesRequestedForDeletion(),
      m_appInstanceUid(QUuid::createUuid()),
      m_propagateCacheEventsFromBackgroundService(false)
{
    Q_CHECK_PTR(cache);
    initialize();
}

/**
 * @brief Shared initialization of constructors.
 */
void Application::initialize()
{
    connect(m_appSettings, &ApplicationSettings::accountsChanged, this,
            &Application::accountsChanged);

    connect(m_cache, &Cache::librariesChanged, this, &Application::onLibrariesChanged);
    m_appSettings->initialize();

    // Connect to background service:
    (void)getBackgroundService();

    // Set up propagation from local cache to the background service
    connect(m_cache, &Cache::dataChanged, this, &Application::onLocalCacheDataChanged);
    connect(m_cache, &Cache::librariesChanged, this, &Application::onLocalCacheLibrariesChanged);
}

/**
 * @brief Internally add the @p library.
 *
 * This method adds the library to the internal list of libraries,
 * ensures it is added to the cache and its content (if already
 * present) loaded fromm disk.
 */
void Application::internallyAddLibrary(Library* library)
{
    auto q = new InsertOrUpdateItemsQuery();
    q->add(library, InsertOrUpdateItemsQuery::Save);
    m_cache->run(q);
    syncLibrary(library);
    library->setCache(m_cache);
    auto libs = m_appSettings->librariesFromConfig();
    libs << QSharedPointer<Library>(Library::decache(library->encache()));
    m_appSettings->librariesToConfig(libs);
}

/**
 * @brief Check if the given @p uid refers to a known library.
 */
bool Application::isLibraryUid(const QUuid& uid)
{
    return m_appSettings->libraryById(uid) != nullptr;
}

QSharedPointer<BackgroundServiceReplica> Application::getBackgroundService()
{
    if (!m_backgroundService || !m_backgroundService->isReplicaValid()) {
#ifdef Q_OS_ANDROID
        {
            QAndroidJniExceptionCleaner cleaner;
            QAndroidJniObject::callStaticMethod<void>(
                    "net/rpdev/OpenTodoList/BackgroundService", "startQtAndroidService",
                    "(Landroid/content/Context;)V", QtAndroid::androidActivity().object());
        }
#endif

        m_remoteObjectNode->connectToNode(QUrl(QStringLiteral("local:opentodolist")));
        m_backgroundService.reset(m_remoteObjectNode->acquire<BackgroundServiceReplica>());
        if (!m_backgroundService->waitForSource(5000)) {
            qCWarning(log) << "Failed to connect to background service";
            m_backgroundService.clear();
        } else {
            connect(m_backgroundService.data(), &BackgroundServiceReplica::librarySyncStarted, this,
                    &Application::onLibrarySyncStarted);
            connect(m_backgroundService.data(), &BackgroundServiceReplica::librarySyncFinished,
                    this, &Application::onLibrarySyncFinished);
            connect(m_backgroundService.data(), &BackgroundServiceReplica::librarySyncError, this,
                    &Application::onLibrarySyncError);
            connect(m_backgroundService.data(), &BackgroundServiceReplica::libraryDeleted, this,
                    &Application::onLibraryDeleted);
            connect(m_backgroundService.data(), &BackgroundServiceReplica::librarySyncProgress,
                    this, &Application::onLibrarySyncProgress);
            connect(m_backgroundService.data(), &BackgroundServiceReplica::cacheDataChanged, this,
                    &Application::onBackgroundServiceCacheDataChanged);
            connect(m_backgroundService.data(), &BackgroundServiceReplica::cacheLibrariesChanged,
                    this, &Application::onBackgroundServiceCacheLibrariesChanged);
            connect(m_backgroundService.data(), &BackgroundServiceReplica::showAppWindowRequested,
                    this, &Application::showWindowRequested);
            connect(m_backgroundService.data(), &BackgroundServiceReplica::serviceAboutToExit,
                    QCoreApplication::instance(), &QCoreApplication::quit);
        }
    }
    return m_backgroundService;
}

void Application::onLibrarySyncStarted(const QUuid& libraryUid)
{
    auto lib = m_appSettings->libraryById(libraryUid);
    if (lib) {
        m_directoriesWithRunningSync.append(lib->directory());
        emit directoriesWithRunningSyncChanged();
    }
}

template<typename T>
void Application::watchLibraryForChanges(T library)
{
    // Forward watching to background service - GUI will use cache to sync up
    if (library) {
        auto backgroundService = getBackgroundService();
        if (backgroundService) {
            backgroundService->watchLibraryDirectory(library->uid());
        }
    }
}

/**
 * @brief Destructor.
 */
Application::~Application() {}

/**
 * @brief Save the Account to the application configuration.
 *
 * This saves the @p account to the application settings.
 */
void Application::saveAccount(Account* account)
{
    m_appSettings->saveAccount(account);
}

/**
 * @brief Save the secrets of the @p account.
 */
void Application::saveAccountSecrets(Account* account)
{
    if (account != nullptr) {
        m_appSettings->saveAccountSecrets(account);
        emit accountsChanged();

        auto backgroundService = getBackgroundService();
        if (backgroundService) {
            backgroundService->setAccountSecret(account->uid(), account->password());
        }

        // Check if the account we just saved credentials for was previously missing
        // credentials and hence remove the problem:
        for (const auto& problem : m_problemManager->problems()) {
            QSharedPointer<Account> problemAccount =
                    qSharedPointerObjectCast<Account>(problem.contextObject());
            if (problemAccount && problemAccount->uid() == account->uid()) {
                m_problemManager->removeProblem(problem);
                break;
            }
        }
    }
}

/**
 * @brief Remove the account from the application.
 *
 * This removes the settings of the account. Additionally,
 * all libraries, belonging to that account will be removed, too.
 */
void Application::removeAccount(Account* account)
{
    if (account != nullptr) {
        for (auto& lib : m_appSettings->librariesFromConfig()) {
            QScopedPointer<Synchronizer> sync(lib->createSynchronizer());
            if (sync) {
                if (sync->accountUid() == account->uid()) {
                    qCDebug(log) << "Removing library" << lib->name() << lib->uid()
                                 << "as it belongs to account" << account->name() << account->uid()
                                 << "which is removed";
                    deleteLibrary(lib.data());
                }
            }
        }

        m_settings->beginGroup("Accounts");
        m_settings->beginGroup(account->uid().toString());
        for (const auto& key : m_settings->allKeys()) {
            m_settings->remove(key);
        }
        m_keyStore->deleteCredentials(account->uid().toString());
        m_settings->endGroup();
        m_settings->endGroup();
        emit accountsChanged();
    }
}

/**
 * @brief Load an account from the app settings.
 *
 * This loads the account with the given @p uid from the settings of
 * the app. If no such account is stored, the function returns a
 * null pointer.
 *
 * @note Owenership goes over to the caller.
 */
Account* Application::loadAccount(const QUuid& uid)
{
    return m_appSettings->loadAccount(uid);
}

/**
 * @brief Get the list of UIDs of the accounts.
 */
QVariantList Application::accountUids()
{
    return m_appSettings->accountUids();
}

/**
 * @brief Create a new local library.
 *
 * This will create a new library, which is stored locally in the
 * default library location.
 */
Library* Application::addLocalLibrary(const QString& name)
{
    Library* result = nullptr;
    auto uid = QUuid::createUuid();
    QDir dir(Library::defaultLibrariesLocation());
    if (!dir.exists() && !dir.mkpath(".")) {
        qCWarning(log) << "Failed to create libraries location in"
                       << Library::defaultLibrariesLocation();
    } else if (dir.mkdir(uid.toString())) {
        auto path = dir.absoluteFilePath(uid.toString());
        result = new Library(path);
        result->setName(name);
        result->setUid(uid);
        result->save();

        watchLibraryForChanges(result);
        internallyAddLibrary(result);
    } else {
        qCWarning(log) << "Failed to create directory for new "
                          "library in "
                       << Library::defaultLibrariesLocation();
    }

    return result;
}

/**
 * @brief Add a local @p directory as a library.
 *
 * This method adds the local @p directory as a library to the application.
 * If the directory points to an existing library location, the library
 * is loaded as-is. Otherwise, a new library is initialized in this
 * directory.
 */
Library* Application::addLibraryDirectory(const QString& directory)
{
    Library* result = nullptr;
    QDir dir(directory);
    if (dir.exists()) {
        if (isLibraryDir(QUrl::fromLocalFile(directory))) {
            result = new Library(directory);
            if (result->load()) {
                auto existingLib = m_appSettings->libraryById(result->uid());
                if (existingLib == nullptr) {
                    internallyAddLibrary(result);
                    watchLibraryForChanges(result);
                } else {
                    qCWarning(log) << "Library in" << directory << "is already register";
                    delete result;
                    result = Library::decache(existingLib->encache());
                    result->setCache(m_cache);
                    // Return here - we do not want to add this a second time
                    // to our list:
                    return result;
                }
            } else {
                qCWarning(log) << "Failed to load library from" << directory;
                delete result;
                result = nullptr;
            }
        } else {
            auto uid = QUuid::createUuid();
            QDir dir_(directory);
            if (dir_.mkdir(uid.toString())) {
                result = new Library(dir_.absoluteFilePath(uid.toString()));
                result->setUid(uid);
                internallyAddLibrary(result);
                watchLibraryForChanges(result);
            } else {
                qCWarning(log) << "Failed to create new library folder in" << directory;
            }
        }
    } else {
        qCWarning(log) << "Cannot add" << directory << "as library: It does not exist";
    }
    return result;
}

/**
 * @brief Add a new library to the given account.
 *
 * This methods adds a new library to the @p account. The library is given the specified @p
 * name. Returns the newly created library.
 *
 * @note The caller takes ownership of the returned object.
 */
Library* Application::addNewLibraryToAccount(Account* account, const QString& name)
{
    Library* result = nullptr;
    if (account && !name.isEmpty()) {
        auto uid = QUuid::createUuid();
        QDir dir(Library::defaultLibrariesLocation());
        if (!dir.exists() && !dir.mkpath(".")) {
            qCWarning(log) << "Failed to create libraries location in"
                           << Library::defaultLibrariesLocation();
        } else if (dir.mkdir(uid.toString())) {
            auto path = dir.absoluteFilePath(uid.toString());
            result = new Library(path);
            result->setName(name);
            result->setUid(uid);
            result->save();

            switch (account->type()) {
            case Account::NextCloud:
            case Account::OwnCloud:
            case Account::WebDAV: {
                WebDAVSynchronizer sync;
                sync.setDirectory(result->directory());
                sync.setAccountUid(account->uid());
                sync.setServerType(account->toWebDAVServerType()
                                           .value<WebDAVSynchronizer::WebDAVServerType>());
                sync.setRemoteDirectory("OpenTodoList/" + result->uid().toString() + ".otl");
                sync.setCreateDirs(true);
                sync.save();
                break;
            }
            case Account::Invalid:
                qCWarning(log) << "Invalid account" << account << "passed to addNewLibraryToAccount"
                               << "function";
            }

            watchLibraryForChanges(result);
            internallyAddLibrary(result);
        } else {
            qCWarning(log) << "Failed to create directory for new "
                              "library in "
                           << Library::defaultLibrariesLocation();
        }
    }
    return result;
}

Library* Application::addExistingLibraryToAccount(Account* account,
                                                  const SynchronizerExistingLibrary& library)
{
    Library* result = nullptr;
    if (account && !library.uid().isNull() && !isLibraryUid(library.uid())) {
        auto uid = library.uid();
        QDir dir(Library::defaultLibrariesLocation());

        if (!dir.exists() && !dir.mkpath(".")) {
            qCWarning(log) << "Failed to create libraries location in"
                           << Library::defaultLibrariesLocation();
        } else {
            auto dirUid = uid;
            if (!dir.mkdir(dirUid.toString())) {
                dirUid = QUuid::createUuid();
                if (!dir.mkdir(dirUid.toString())) {
                    qCWarning(log) << "Failed to create a directory for the library in" << dir;
                    return nullptr;
                }
            }
            auto path = dir.absoluteFilePath(dirUid.toString());
            result = new Library(path);
            result->setName(library.name());
            result->setUid(uid);
            result->save();

            switch (account->type()) {
            case Account::NextCloud:
            case Account::OwnCloud:
            case Account::WebDAV: {
                WebDAVSynchronizer sync;
                sync.setDirectory(result->directory());
                sync.setAccountUid(account->uid());
                sync.setServerType(account->toWebDAVServerType()
                                           .value<WebDAVSynchronizer::WebDAVServerType>());
                sync.setRemoteDirectory(library.path());
                sync.save();
                break;
            }
            case Account::Invalid:
                qCWarning(log) << "Invalid account" << account << "passed to addNewLibraryToAccount"
                               << "function";
            }

            watchLibraryForChanges(result);
            internallyAddLibrary(result);
        }
    }
    return result;
}

/**
 * @brief Delete the library.
 *
 * This will remove the @p library from the application, i.e.
 * it will be removed from the cache. In addition, if the library
 * is located in the default libraries location, the files on
 * disk stored will also be removed.
 */
void Application::deleteLibrary(Library* library)
{
    if (library != nullptr) {
        // Inform the background service about the removal:
        auto backgroundService = getBackgroundService();
        if (backgroundService) {
            backgroundService->deleteLibrary(library->uid());
        }

        m_librariesRequestedForDeletion.insert(library->uid());
    }
}

Note* Application::addNote(Library* library, QVariantMap properties)
{
    Note* note = nullptr;
    if (library != nullptr) {
        if (library->isValid()) {
            QDir dir(library->newItemLocation());
            dir.mkpath(".");
            note = new Note(dir);
        } else {
            note = new Note();
        }
        for (auto property : properties.keys()) {
            auto value = properties.value(property);
            note->setProperty(property.toUtf8(), value);
        }
        note->setLibraryId(library->uid());
        auto q = new InsertOrUpdateItemsQuery();
        q->add(note, InsertOrUpdateItemsQuery::CreateNewItem);
        m_cache->run(q);
        note->setCache(m_cache);
    }
    return note;
}

NotePage* Application::addNotePage(Library* library, Note* note, QVariantMap properties)
{
    NotePage* page = nullptr;
    if (library != nullptr && note != nullptr) {
        if (library->isValid()) {
            QDir dir(library->newItemLocation());
            dir.mkpath(".");
            page = new NotePage(dir);
        } else {
            page = new NotePage();
        }
        for (auto property : properties.keys()) {
            auto value = properties.value(property);
            page->setProperty(property.toUtf8(), value);
        }
        page->setNoteUid(note->uid());
        auto q = new InsertOrUpdateItemsQuery();
        q->add(page, InsertOrUpdateItemsQuery::CreateNewItem);
        m_cache->run(q);
        page->setCache(m_cache);
    }
    return page;
}

Image* Application::addImage(Library* library, QVariantMap properties)
{
    Image* image = nullptr;
    if (library != nullptr) {
        if (library->isValid()) {
            QDir dir(library->newItemLocation());
            dir.mkpath(".");
            image = new Image(dir);
        } else {
            image = new Image();
        }
        for (auto property : properties.keys()) {
            auto value = properties.value(property);
            image->setProperty(property.toUtf8(), value);
        }
        image->setLibraryId(library->uid());
        auto q = new InsertOrUpdateItemsQuery();
        q->add(image, InsertOrUpdateItemsQuery::CreateNewItem);
        m_cache->run(q);
        image->setCache(m_cache);
    }
    return image;
}

TodoList* Application::addTodoList(Library* library, QVariantMap properties)
{
    TodoList* todoList = nullptr;
    if (library != nullptr) {
        if (library->isValid()) {
            QDir dir(library->newItemLocation());
            dir.mkpath(".");
            todoList = new TodoList(dir);
        } else {
            todoList = new TodoList();
        }
        for (auto property : properties.keys()) {
            auto value = properties.value(property);
            todoList->setProperty(property.toUtf8(), value);
        }
        todoList->setLibraryId(library->uid());
        auto q = new InsertOrUpdateItemsQuery();
        q->add(todoList, InsertOrUpdateItemsQuery::CreateNewItem);
        m_cache->run(q);
        todoList->setCache(m_cache);
    }
    return todoList;
}

Todo* Application::addTodo(Library* library, TodoList* todoList, QVariantMap properties)
{
    Todo* todo = nullptr;
    if (library != nullptr && todoList != nullptr) {
        if (library->isValid()) {
            QDir dir(library->newItemLocation());
            dir.mkpath(".");
            todo = new Todo(dir);
        } else {
            todo = new Todo();
        }
        for (auto property : properties.keys()) {
            auto value = properties.value(property);
            todo->setProperty(property.toUtf8(), value);
        }
        todo->setTodoListUid(todoList->uid());
        auto q = new InsertOrUpdateItemsQuery();
        q->add(todo, InsertOrUpdateItemsQuery::CreateNewItem);
        m_cache->run(q);
        todo->setCache(m_cache);
    }
    return todo;
}

/**
 * @brief Move the @p todo into the specified @p todoList.
 */
void Application::moveTodo(Todo* todo, TodoList* todoList)
{
    if (todo && todoList) {
        auto q = new MoveTodoQuery();
        q->moveTodo(todo, todoList);
        m_cache->run(q);
    }
}

Task* Application::addTask(Library* library, Todo* todo, QVariantMap properties)
{
    Task* task = nullptr;
    if (library != nullptr && todo != nullptr) {
        if (library->isValid()) {
            QDir dir(library->newItemLocation());
            dir.mkpath(".");
            task = new Task(dir);
        } else {
            task = new Task();
        }
        for (auto property : properties.keys()) {
            auto value = properties.value(property);
            task->setProperty(property.toUtf8(), value);
        }
        task->setTodoUid(todo->uid());
        auto q = new InsertOrUpdateItemsQuery();
        q->add(task, InsertOrUpdateItemsQuery::CreateNewItem);
        m_cache->run(q);
        task->setCache(m_cache);
    }
    return task;
}

/**
 * @brief Promote a task to a todo.
 *
 * This converts the given @p task to a todo, moving it to the @p targetTodoList.
 */
void Application::promoteTask(Task* task, TodoList* targetTodoList)
{
    if (task && targetTodoList) {
        auto q = new PromoteTaskQuery();
        q->promoteTask(task, targetTodoList);
        m_cache->run(q);
    }
}

void Application::deleteItem(Item* item)
{
    if (item != nullptr) {
        auto q = new DeleteItemsQuery();
        q->deleteItem(item);
        m_cache->run(q);
    }
}

/**
 * @brief Delete all todos from a todo list that are done.
 */
void Application::deleteDoneTodos(TodoList* todoList)
{
    if (todoList != nullptr) {
        auto q = new DeleteItemsQuery();
        q->deleteChildrenOfItem(todoList, [](const Item* item) {
            auto todo = qobject_cast<const Todo*>(item);
            return todo && todo->done();
        });
        m_cache->run(q);
    }
}

/**
 * @brief Delete all taks from a todo that are done.
 */
void Application::deleteDoneTasks(Todo* todo)
{
    if (todo != nullptr) {
        auto q = new DeleteItemsQuery();
        q->deleteChildrenOfItem(todo, [](const Item* item) {
            auto task = qobject_cast<const Task*>(item);
            return task && task->done();
        });
        m_cache->run(q);
    }
}

/**
 * @brief Request loading an item from the cache.
 *
 * This method triggers loading the item with the given @p uid from the cache. Once the
 * item data is available, the itemLoaded() signal is emitted.
 *
 * If the uid is null, this method has no effect.
 */
void Application::loadItem(const QUuid& uid)
{
    if (!uid.isNull()) {
        auto q = new GetItemQuery();
        q->setUid(uid);
        QPointer<Application> _this = this;
        connect(q, &GetItemQuery::itemLoaded, [_this, uid](const QVariant& data) {
            if (_this) {
                QMetaObject::invokeMethod(
                        _this,
                        [uid, data, _this]() {
                            if (_this) {
                                emit _this->itemLoaded(uid, data);
                            }
                        },
                        Qt::QueuedConnection);
            }
        });
        m_cache->run(q);
    }
}

/**
 * @brief Create and return an item from cached data.
 *
 * This method takes the serialized item @p data and returns an item created from the data.
 * If the data is invalid, a null pointer is returned. If creating an item was succesfull, the
 * resulting item is set up to automatically update from the cache in case of changes.
 *
 * @note Ownership is transferred to the caller.
 */
Item* Application::itemFromData(const QVariant& data)
{
    auto item = Item::decache(data);
    if (item) {
        item->setCache(m_cache);
    }
    return item;
}

/**
 * @brief Clone an item.
 *
 * This creates a clone of the given @p item. In addition, the resulting item will be setup to track
 * changes in the item cache, so whenever ther are any updates (e.g. due to sync) the item gets
 * updated as well.
 */
Item* Application::cloneItem(Item* item)
{
    Item* result = nullptr;
    if (item != nullptr) {
        result = Item::decache(item->encache());
        result->setCache(m_cache);
    }
    return result;
}

/**
 * @brief Create a snapshot of the item and store it for later.
 *
 * This saves the current state of the @p item and returns a snapshot of it. This data can be used
 * to e.g. restore the item state.
 */
QString Application::saveItem(Item* item)
{
    QByteArray result;
    if (item != nullptr) {
        result = item->encache().serialize().toHex();
    }
    return result;
}

/**
 * @brief Restore a previously saved item.
 *
 * This restores the state of an item from the @p data, which previously must have been created via
 * a call to saveItem().
 */
void Application::restoreItem(const QString& data)
{
    QSharedPointer<Item> item(
            Item::decache(ItemCacheEntry::deserialize(QByteArray::fromHex(data.toUtf8()))));
    qCWarning(log) << "Restore item" << data << item;
    if (item != nullptr) {
        auto q = new InsertOrUpdateItemsQuery();
        q->add(item.data(), InsertOrUpdateItemsQuery::Save);
        m_cache->run(q);
    }
}

/**
 * @brief Save a value to the application settings
 *
 * This method is used to save a value to the application settings. Settings
 * can be restored (e.g. when the app restarts).
 */
void Application::saveValue(const QString& name, const QVariant& value)
{
    m_settings->beginGroup("ApplicationSettings");
    m_settings->setValue(name, value);
    m_settings->endGroup();
}

/**
 * @brief Read a persistent application value
 *
 * This method is used to read back persistent application settings which
 * previously have been written using saveValue().
 */
QVariant Application::loadValue(const QString& name, const QVariant& defaultValue)
{
    m_settings->beginGroup("ApplicationSettings");
    QVariant result = m_settings->value(name, defaultValue);
    m_settings->endGroup();
    return result;
}

/**
 * @brief A list of all 3rd party information found in the apps resource system.
 */
QVariant Application::find3rdPartyInfos() const
{
    QDirIterator it(":/", { "3rdpartyinfo.json" }, QDir::Files, QDirIterator::Subdirectories);
    QVariantList result;
    while (it.hasNext()) {
        QString file = it.next();
        bool ok;
        auto map = JsonUtils::loadMap(file, &ok);
        if (ok) {
            result.append(map);
        }
    }
    return result;
}

bool Application::saveTextToFile(const QUrl& fileUrl, const QString& text) const
{
    if (fileUrl.isValid() && fileUrl.isLocalFile()) {
        auto localPath = fileUrl.toLocalFile();
        QFile outputFile(localPath);
        if (outputFile.open(QIODevice::WriteOnly)) {
            outputFile.write(text.toUtf8());
            outputFile.close();
            return true;
        } else {
            qCWarning(log) << "Failed to open" << localPath
                           << "for writing:" << outputFile.errorString();
        }
    }
    return false;
}

/**
 * @brief Convert a URL to a local file name.
 */
QString Application::urlToLocalFile(const QUrl& url) const
{
    return url.toLocalFile();
}

/**
 * @brief Convert a local file name to a url.
 */
QUrl Application::localFileToUrl(const QString& localFile) const
{
    return QUrl::fromLocalFile(localFile);
}

/**
 * @brief Clean the local file path given via the @p url.
 *
 * This method removes extra dots and dotdots from the file path
 * and returns a simplified verion of the input path.
 */
QUrl Application::cleanPath(const QUrl& url) const
{
    auto path = url.toLocalFile();
    path = QDir::cleanPath(path);
    return QUrl::fromLocalFile(path);
}

/**
 * @brief Converts HTML into plain text.
 *
 * This function gets an @p html string as input and returns the text converted
 * to plain text.
 */
QString Application::htmlToPlainText(const QString& html) const
{
    QTextDocument doc;
    doc.setHtml(html);
    return doc.toPlainText();
}

/**
 * @brief Check if a file called @p filename exists.
 */
bool Application::fileExists(const QString& filename) const
{
    return QFile(filename).exists();
}

/**
 * @brief Check if the @p directory exists.
 */
bool Application::directoryExists(const QString& directory) const
{
    return !directory.isEmpty() && QDir(directory).exists();
}

/**
 * @brief Get the basename of the @p filename.
 */
QString Application::basename(const QString& filename) const
{
    return QFileInfo(filename).baseName();
}

/**
 * @brief Test if the @p url points to an existing library directory.
 */
bool Application::isLibraryDir(const QUrl& url) const
{
    bool result = false;
    if (url.isValid()) {
        auto dir = QDir(url.toLocalFile());
        if (dir.exists() && dir.exists(Library::LibraryFileName)) {
            result = true;
        }
    }
    return result;
}

/**
 * @brief Get the name of a library from its directory.
 *
 * This method takes the path to a library and - if the path points to a valid
 * library folder - returns the name of it. When the directory is not a library folder, this
 * returns an empty string.
 */
QString Application::libraryNameFromDir(const QUrl& url) const
{
    if (isLibraryDir(url)) {
        auto path = url.toLocalFile();
        Library lib(path);
        if (lib.load()) {
            return lib.name();
        }
    }
    return QString();
}

/**
 * @brief Get the SHA256 hash over the @p text.
 */
QString Application::sha256(const QString& text) const
{
    return QCryptographicHash::hash(text.toUtf8(), QCryptographicHash::Sha256).toHex();
}

/**
 * @brief Returns the home location of the current user.
 */
QUrl Application::homeLocation() const
{
    QString homeDir = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    return QUrl::fromLocalFile(homeDir);
}

/**
 * @brief Returns true of the folder pointed to by @p url exists.
 */
bool Application::folderExists(const QUrl& url) const
{
    return url.isValid() && QDir(url.toLocalFile()).exists();
}

/**
 * @brief Check if the @p uid refers to an existing library in the app.
 */
bool Application::libraryExists(const QUuid& uid)
{
    return isLibraryUid(uid);
}

/**
 * @brief Create a new flow to log in to NextCloud.
 *
 * This returns a new object which can be used to log in to a NextCloud instance via the
 * NextCloud Login flow.
 *
 * If ignoreSslErrors is true, the resulting object will ignore any issues regarding SSL.
 *
 * The caller takes ownership of the created object.
 */
SynqClient::NextCloudLoginFlow* Application::createNextCloudLoginFlow(bool ignoreSslErrors) const
{
    auto result = new SynqClient::NextCloudLoginFlow();
    auto nam = new QNetworkAccessManager(result);
    result->setNetworkAccessManager(nam);
    result->setUserAgent(Synchronizer::HTTPUserAgent);
    if (ignoreSslErrors) {
        connect(nam, &QNetworkAccessManager::sslErrors, result,
                [=](QNetworkReply* reply, const QList<QSslError>& errors) {
                    reply->ignoreSslErrors(errors);
                });
    }
    return result;
}

/**
 * @brief Trigger a sync for all libraries.
 */
void Application::syncAllLibraries()
{
    for (const auto& library : m_appSettings->librariesFromConfig()) {
        if (library->hasSynchronizer()) {
            runSyncForLibrary(library);
        }
    }
}

#ifdef Q_OS_ANDROID
void Application::finishActivity()
{
    auto activity = QtAndroid::androidActivity();
    QtAndroid::runOnAndroidThread(
            [=]() { activity.callMethod<void>("finish", "()V", activity.object()); });
}
#endif

/**
 * @brief Start synchronizing the @p library.
 */
void Application::syncLibrary(Library* library)
{
    runSyncForLibrary(library);
}

/**
 * @brief Copy the @p text to the clipboard.
 */
void Application::copyToClipboard(const QString& text)
{
    auto app = qobject_cast<QGuiApplication*>(qApp);
    if (app != nullptr) {
        auto clipboard = app->clipboard();
        clipboard->setText(text);
    }
}

void Application::clearSyncErrors(Library* library)
{
    if (library) {
        m_syncErrors.remove(library->directory());
        emit syncErrorsChanged();
        m_problemManager->removeProblemsFor(library->uid(), Problem::SyncFailed);
    }
}

void Application::setPropagateCacheEventsFromBackgroundService(
        bool propagateCacheEventsFromBackgroundService)
{
    m_propagateCacheEventsFromBackgroundService = propagateCacheEventsFromBackgroundService;
}

ProblemManager* Application::problemManager() const
{
    return m_problemManager;
}

QVariantMap Application::syncErrors() const
{
    return m_syncErrors;
}

QVariantMap Application::syncProgress() const
{
    return m_syncProgress;
}

/**
 * @brief The list of directories in which a sync is currently running.
 */
QStringList Application::directoriesWithRunningSync() const
{
    return m_directoriesWithRunningSync;
}

Cache* Application::cache() const
{
    return m_cache;
}

/**
 * @brief Get the location where libraries are stored by default.
 */
QString Application::librariesLocation() const
{
    return Library::defaultLibrariesLocation();
}

void Application::onLibrarySyncFinished(const QUuid& libraryUid)
{
    auto lib = m_appSettings->libraryById(libraryUid);
    if (lib && lib->isValid()) {
        QScopedPointer<Synchronizer> sync(lib->createSynchronizer());
        // Unmark directory as having running sync:
        m_directoriesWithRunningSync.removeAll(lib->directory());
        emit directoriesWithRunningSyncChanged();

        // Only if we know this library is not already requested to be deleted:
        if (!m_librariesRequestedForDeletion.contains(libraryUid)) {
            // Save last sync date:
            if (sync) {
                sync->setLastSync(QDateTime::currentDateTime());
                sync->save();
            }

            // If there were changes in the meantime, run another sync right away:
            if (m_librariesWithChanges.contains(lib->directory())) {
                m_librariesWithChanges.remove(lib->directory());
                runSyncForLibrary(lib);
            }
        }
    }
}

void Application::onLibrarySyncError(const QUuid& libraryUid, const QString& error)
{
    auto lib = m_appSettings->libraryById(libraryUid);
    if (lib) {
        auto errors = m_syncErrors.value(lib->directory(), QStringList()).toStringList();
        errors.append(error);
        m_syncErrors[lib->directory()] = errors;
        emit syncErrorsChanged();

        m_problemManager->removeProblemsFor(libraryUid, Problem::SyncFailed);
        Problem problem;
        problem.setContextObject(lib);
        problem.setMessage(error);
        problem.setType(Problem::SyncFailed);
        m_problemManager->addProblem(problem);
    }
}

void Application::onLibrarySyncProgress(const QUuid& libraryUid, int value)
{
    auto lib = m_appSettings->libraryById(libraryUid);
    if (lib) {
        if (!m_directoriesWithRunningSync.contains(lib->directory())) {
            m_directoriesWithRunningSync.append(lib->directory());
            emit directoriesWithRunningSyncChanged();
        }
        m_syncProgress[lib->directory()] = value;
        emit syncProgressChanged();
    }
}

void Application::onLibraryDeleted(const QUuid& libraryUid)
{
    if (m_librariesRequestedForDeletion.contains(libraryUid)) {
        m_librariesRequestedForDeletion.remove(libraryUid);
    }
}

void Application::onLibrariesChanged(QVariantList librariesUids)
{
    auto libs = m_appSettings->librariesFromConfig();
    for (auto id : librariesUids) {
        auto uid = id.toUuid();
        for (auto lib : libs) {
            if (uid == lib->uid()) {
                if (m_directoriesWithRunningSync.contains(lib->directory())) {
                    m_librariesWithChanges.insert(lib->directory());
                } else {
                    runSyncForLibrary(lib);
                }
            }
        }
    }
}

/**
 * @brief Propagate dataChanged() signals from cache in background service.
 */
void Application::onBackgroundServiceCacheDataChanged(const QUuid& appInstanceUid)
{
    if (m_propagateCacheEventsFromBackgroundService && appInstanceUid != m_appInstanceUid) {
        emit m_cache->dataChanged();
    }
}

/**
 * @brief Propagate librariesChanged() signals from cache in background service.
 */
void Application::onBackgroundServiceCacheLibrariesChanged(const QVariantList& libraryUids,
                                                           const QUuid& appInstanceUid)
{
    if (m_propagateCacheEventsFromBackgroundService && appInstanceUid != m_appInstanceUid) {
        emit m_cache->librariesChanged(libraryUids);
    }
}

/**
 * @brief Inform the background service about changes in the cache.
 *
 * If propagation of sync events is on, notify the background service about the changes of data
 * in the cache. The service will notify all clients about this, so they can update views.
 */
void Application::onLocalCacheDataChanged()
{
    if (m_propagateCacheEventsFromBackgroundService) {
        auto backgroundService = getBackgroundService();
        if (backgroundService) {
            backgroundService->notifyCacheDataChanged(m_appInstanceUid);
        }
    }
}

/**
 * @brief Inform the background service about changes in the cache.
 *
 * If propagation of sync events is on, notify the background service about the changes of data
 * in the cache. The service will notify all clients about this, so they can update views.
 */
void Application::onLocalCacheLibrariesChanged(const QVariantList& libraryUids)
{
    if (m_propagateCacheEventsFromBackgroundService) {
        auto backgroundService = getBackgroundService();
        if (backgroundService) {
            backgroundService->notifyCacheLibrariesChanged(libraryUids, m_appInstanceUid);
        }
    }
}

template<typename T>
void Application::runSyncForLibrary(T library)
{
    if (library != nullptr && library->isValid()
        && !m_librariesRequestedForDeletion.contains(library->uid())) {
        if (!m_directoriesWithRunningSync.contains(library->directory())) {
            auto backgroundService = getBackgroundService();
            if (backgroundService) {
                m_syncErrors.remove(library->directory());
                emit syncErrorsChanged();
                m_problemManager->removeProblemsFor(library->uid(), Problem::SyncFailed);
                backgroundService->syncLibrary(library->uid());
            } else {
                qCWarning(log) << "Cannot sync library: Failed to connect to background service";
            }
        }
    }
}

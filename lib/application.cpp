#include "application.h"

#include <QClipboard>
#include <QCoreApplication>
#include <QDebug>
#include <QDateTime>
#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QGuiApplication>
#include <QJsonDocument>
#include <QProcess>
#include <QScopedPointer>
#include <QStandardPaths>
#include <QTextDocument>
#include <QThreadPool>
#include <QTimer>
#include <QUuid>

#include "datamodel/image.h"
#include "datamodel/note.h"
#include "datamodel/task.h"
#include "datamodel/todo.h"
#include "datamodel/todolist.h"
#include "datastorage/cache.h"
#include "datastorage/deleteitemsquery.h"
#include "datastorage/insertorupdateitemsquery.h"
#include "datastorage/libraryloader.h"
#include "sync/synchronizer.h"
#include "sync/syncjob.h"
#include "sync/syncrunner.h"
#include "sync/webdavsynchronizer.h"
#include "utils/jsonutils.h"
#include "utils/keystore.h"


static Q_LOGGING_CATEGORY(log, "OpenTodoList.Application", QtDebugMsg)


/**
 * @brief Constructor.
 *
 * Creates a new Application object. The instance will be a child of the given
 * @p parent.
 */
Application::Application(QObject *parent) :
    QObject(parent),
    m_settings(new QSettings(QSettings::IniFormat, QSettings::UserScope,
                             QCoreApplication::organizationName(),
                             QCoreApplication::applicationName(), this)),
    m_cache(new Cache(this)),
    m_keyStore(new KeyStore(this)),
    m_secrets()
{
    initialize();
}


/**
 * @brief Constructor.
 *
 * This is an overloaded version of the Application constructor. It causes the
 * Application object to store its settings in the given @p applicationDir.
 */
Application::Application(QString applicationDir, QObject *parent) :
    QObject(parent),
    m_settings(new QSettings(applicationDir + "/appsettings.ini",
                            QSettings::IniFormat, this)),
    m_cache(new Cache(this)),
    m_keyStore(new KeyStore(this)),
    m_secrets()
{
    initialize(applicationDir);
}

/**
 * @brief Shared initialization of constructors.
 */
void Application::initialize(const QString &path)
{
    auto cacheDir = path;
    if (cacheDir.isEmpty()) {
        cacheDir = QStandardPaths::writableLocation(
                    QStandardPaths::AppDataLocation);
    }
    cacheDir += "/cache";
    {
        QDir dir(cacheDir);
        if (!dir.exists()) {
            dir.mkpath(".");
        }
    }
    m_cache->setCacheDirectory(cacheDir);
    m_cache->open();

    loadLibraries();

    connect(m_keyStore, &KeyStore::credentialsLoaded,
            [=](const QString& key, const QString& value, bool success) {
        if (success) {
            if (!m_secrets.contains(key)) {
                m_secrets.insert(key, value);
                emit secretsKeysChanged();
                for (auto lib : librariesFromConfig()) {
                    QScopedPointer<Synchronizer> sync(
                                lib->createSynchronizer());
                    if (sync) {
                        if (sync->secretsKey() == key) {
                            qCDebug(log) << "Start-up sync of"
                                                 << lib << lib->name();
                            runSyncForLibrary(lib);
                        }
                    }
                }
            } else {
                qCWarning(log) << "Received credentials for a key"
                               << key << "but we already have"
                               << "credentials for that one";
            }
        }
    });

    auto syncTimer = new QTimer(this);
    // Check if we need to sync every 5 min
    syncTimer->setInterval(1000 * 60 * 5);
    syncTimer->setSingleShot(false);
    connect(syncTimer, &QTimer::timeout, [=]() {
        for (auto lib : librariesFromConfig()) {
            QScopedPointer<Synchronizer> sync(lib->createSynchronizer());
            if (sync) {
                auto lastSync = sync->lastSync();
                bool runSync = false;
                if (!lastSync.isValid()) {
                    runSync = true;
                } else {
                    auto currentDateTime = QDateTime::currentDateTime();
                    auto diff = currentDateTime.toMSecsSinceEpoch() -
                            lastSync.toMSecsSinceEpoch();
                    if (diff >= (1000 * 60 * 60)) {
                        // Sync every hour
                        qCDebug(log) << "Library" << lib
                                             << lib->name()
                                             << "has not been synced for"
                                             << "more than an hour,"
                                             << "starting sync now";
                        runSync = true;
                    }
                }
                if (runSync) {
                    runSyncForLibrary(lib);
                }
            }
        }
    });
    syncTimer->start();
}


QList<QSharedPointer<Library> > Application::librariesFromConfig()
{
    QList<QSharedPointer<Library>> result;
    auto count = m_settings->beginReadArray("LibraryDirectories");
    for (auto i = 0; i < count; ++i) {
        m_settings->setArrayIndex(i);
        auto directory = m_settings->value("directory").toString();
        if (directory != "" && QDir(directory).exists()) {
            auto lib = QSharedPointer<Library>(new Library(directory));
            if (lib->load()) {
                result << lib;
            } else {
                qCWarning(log) << "Failed to load library from directory"
                               << directory;
            }
        }
    }
    m_settings->endArray();
    return result;
}


void Application::librariesToConfig(QList<QSharedPointer<Library> > libraries)
{
    m_settings->beginWriteArray("LibraryDirectories", libraries.length());
    for (auto i = 0; i < libraries.length(); ++i) {
        m_settings->setArrayIndex(i);
        m_settings->setValue("directory", libraries[i]->directory());
    }
    m_settings->endArray();
}

void Application::syncLibrariesWithCache(
        QList<QSharedPointer<Library> > libraries)
{
    for (auto library : libraries) {
        auto loader = new LibraryLoader(this);
        loader->setCache(m_cache);
        loader->setDirectory(library->directory());
        loader->setLibraryId(library->uid());
        connect(loader, &LibraryLoader::scanFinished,
                loader, &LibraryLoader::deleteLater);
        loader->scan();
    }
}


/**
 * @brief Destructor.
 */
Application::~Application()
{
}


/**
 * @brief Create a new library.
 *
 * This creates a new library using the given @p parameters. The following
 * keys are assumed to be provided (some of them optional):
 *
 * - name: The name of the library.
 * - localPath: The path where to create the library in.
 * - synchronizer: The type (class name) of synchronizer to use.
 * - serverType: For some synchronizer classes, the concrete sub-type of the
 *   server.
 * - uid: The UID to use for the library.
 * - path: The remote path of the library.
 *
 * If no synchronizer is specified and localPath points to a location
 * with an existing library, that library will be imported. In this
 * case, any other attributes are ignored.
 *
 * The function returns a Library object which represents the library just
 * created. The ownership of the returned object is transferred to the caller.
 * In case of invalid input parameters or issues creating the library, a
 * null pointer is returned.
 */
Library *Application::addLibrary(const QVariantMap &parameters)
{
    Library* result = nullptr;

    auto synchronizerCls = parameters.value("synchronizer").toString();
    auto localPath = parameters.value(
                "localPath", librariesLocation()).toString();
    auto name = parameters.value("name").toString();
    QUuid uid;
    if (parameters.contains("uid") &&
            parameters.value("uid").toString() != "") {
        uid = parameters.value("uid").toUuid();
    } else {
        uid = QUuid::createUuid();
    }
    auto path = parameters.value("path").toString();

    if (!isLibraryDir(QUrl::fromLocalFile(localPath)) ||
            localPath == librariesLocation()) {
        auto modLocalPath = localPath + "/" + uid.toString();
        if (QDir(modLocalPath).exists()) {
            // Do not re-use existing directory:
            localPath += "/" + QUuid::createUuid().toString();
        } else {
            localPath = modLocalPath;
        }
    }

    QDir(localPath).mkpath(".");

    if (QDir(localPath).exists()) {
        result = new Library(localPath);
        result->setUid(uid);
        result->setName(name);
        if (!synchronizerCls.isEmpty()) {
            if (synchronizerCls == "WebDAVSynchronizer") {
                auto sync = new WebDAVSynchronizer();
                auto type = parameters.value("serverType")
                        .value<WebDAVSynchronizer::WebDAVServerType>();
                sync->setServerType(type);
                sync->setUrl(parameters.value("url").toUrl());
                sync->setUsername(parameters.value("username").toString());
                sync->setPassword(parameters.value("password").toString());
                sync->setDisableCertificateCheck(
                            parameters.value(
                                "disableCertificateCheck").toBool());
                sync->setDirectory(result->directory());
                if (path.isEmpty()) {
                    path = "OpenTodoList/" + uid.toString() + ".otl";
                    sync->setCreateDirs(true);
                }
                sync->setRemoteDirectory(path);
                sync->save();
                m_keyStore->saveCredentials(sync->secretsKey(), sync->secret());
                m_secrets.insert(sync->secretsKey(), sync->secret());
                emit secretsKeysChanged();
                delete sync;
            }
        }
        auto q = new InsertOrUpdateItemsQuery();
        q->add(result);
        m_cache->run(q);
        syncLibrary(result);
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
void Application::deleteLibrary(Library *library)
{
    if (library != nullptr) {
        if (m_directoriesWithRunningSync.contains(library->directory())) {
            qCWarning(log) << "Cannot delete a library which is syncing.";
            return;
        }
        auto q = new DeleteItemsQuery();
        q->deleteLibrary(library, library->isInDefaultLocation());
        m_cache->run(q);
        auto libs = librariesFromConfig();
        for (auto lib : libs) {
            if (lib->directory() == library->directory()) {
                libs.removeOne(lib);
                break;
            }
        }
        librariesToConfig(libs);
    }
}

Note *Application::addNote(Library *library, QVariantMap properties)
{
    Note *note = nullptr;
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

Image* Application::addImage(Library *library, QVariantMap properties)
{
    Image *image = nullptr;
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

TodoList *Application::addTodoList(Library *library, QVariantMap properties)
{
    TodoList *todoList = nullptr;
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


Todo *Application::addTodo(
        Library *library, TodoList *todoList, QVariantMap properties)
{
    Todo *todo = nullptr;
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

Task *Application::addTask(Library *library, Todo *todo, QVariantMap properties)
{
    Task *task = nullptr;
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


void Application::deleteItem(Item *item)
{
    if (item != nullptr) {
        auto q = new DeleteItemsQuery();
        q->deleteItem(item);
        m_cache->run(q);
    }
}


/**
 * @brief Save a value to the application settings
 *
 * This method is used to save a value to the application settings. Settings
 * can be restored (e.g. when the app restarts).
 */
void Application::saveValue(const QString &name, const QVariant &value)
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
QVariant Application::loadValue(const QString &name,
                                const QVariant &defaultValue)
{
    m_settings->beginGroup("ApplicationSettings");
    QVariant result = m_settings->value(name, defaultValue);
    m_settings->endGroup();
    return result;
}

/**
 * @brief Reads a file and returns its content
 */
QString Application::readFile(const QString &fileName) const
{
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly)) {
        return file.readAll();
    }
    return QString();
}

/**
 * @brief A list of all 3rd party information found in the apps resource system.
 */
QVariant Application::find3rdPartyInfos() const
{
    QDirIterator it(":/", {"3rdpartyinfo.json"},
                    QDir::Files, QDirIterator::Subdirectories);
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

/**
 * @brief Convert a URL to a local file name.
 */
QString Application::urlToLocalFile(const QUrl &url) const
{
    return url.toLocalFile();
}

/**
 * @brief Convert a local file name to a url.
 */
QUrl Application::localFileToUrl(const QString &localFile) const
{
    return QUrl::fromLocalFile(localFile);
}


/**
 * @brief Clean the local file path given via the @p url.
 *
 * This method removes extra dots and dotdots from the file path
 * and returns a simplified verion of the input path.
 */
QUrl Application::cleanPath(const QUrl &url) const
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
QString Application::htmlToPlainText(const QString &html) const
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
bool Application::isLibraryDir(const QUrl &url) const
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
 * @brief Returns the home location of the current user.
 */
QUrl Application::homeLocation() const
{
    QString homeDir = QStandardPaths::writableLocation(
                QStandardPaths::HomeLocation);
    return QUrl::fromLocalFile(homeDir);
}

/**
 * @brief Returns true of the folder pointed to by @p url exists.
 */
bool Application::folderExists(const QUrl &url) const
{
    return url.isValid() && QDir(url.toLocalFile()).exists();
}


/**
 * @brief Get the secrets for a synchronizer.
 *
 * Get the secrets for the synchronizer @p sync. If no secrets
 * for the synchronizer where stored, an empty string is returned.
 */
QString Application::secretForSynchronizer(Synchronizer *sync)
{
    QString result;
    if (sync != nullptr) {
        auto key = sync->secretsKey();
        if (!key.isEmpty() && m_secrets.contains(key)) {
            result = m_secrets.value(key).toString();
        }
    }
    return result;
}


/**
 * @brief Start synchronizing the @p library.
 */
void Application::syncLibrary(Library *library)
{
    runSyncForLibrary(library);
}


/**
 * @brief Save the secrets for a synchronizer.
 *
 * This saves the secrets of the given Synchronizer @p sync to the
 * key store.
 */
void Application::saveSynchronizerSecrets(Synchronizer *sync)
{
    if (sync != nullptr) {
        auto key = sync->secretsKey();
        if (!key.isEmpty()) {
            auto secret = sync->secret();
            m_keyStore->saveCredentials(key, secret);
            m_secrets[key] = secret;
            emit secretsKeysChanged();
        }
    }
}


/**
 * @brief Copy the @p text to the clipboard.
 */
void Application::copyToClipboard(const QString &text)
{
    auto app = dynamic_cast<QGuiApplication*>(qApp);
    if (app != nullptr) {
        auto clipboard = app->clipboard();
        clipboard->setText(text);
    }
}

QVariantMap Application::syncErrors() const
{
    return m_syncErrors;
}


/**
 * @brief A list of secrets keys.
 *
 * @sa Library::synchronizerSecret
 */
QStringList Application::secretsKeys() const
{
    return m_secrets.keys();
}


/**
 * @brief The list of directories in which a sync is currently running.
 */
QStringList Application::directoriesWithRunningSync() const
{
    return m_directoriesWithRunningSync;
}


/**
 * @brief Set the list of directories in which currently a sync is running.
 */
void Application::setDirectoriesWithRunningSync(
        const QStringList &directoriesWithRunningSync)
{
    if (m_directoriesWithRunningSync != directoriesWithRunningSync) {
        m_directoriesWithRunningSync = directoriesWithRunningSync;
        emit directoriesWithRunningSyncChanged();
    }
}

Cache *Application::cache() const
{
    return m_cache;
}


void Application::loadLibraries()
{
    m_secrets.clear();
    emit secretsKeysChanged();

    for (auto library : librariesFromConfig()) {
        auto loader = new LibraryLoader();
        loader->setDirectory(library->directory());
        loader->setLibraryId(library->uid());
        loader->setCache(m_cache);
        connect(loader, &LibraryLoader::scanFinished,
                loader, &LibraryLoader::deleteLater);
        loader->scan();
        QScopedPointer<Synchronizer> sync(library->createSynchronizer());
        if (sync) {
            auto key = sync->secretsKey();
            if (!key.isEmpty()) {
                m_keyStore->loadCredentials(key);
            }
        }
    }
}


/**
 * @brief Get the location where libraries are stored by default.
 */
QString Application::librariesLocation() const
{
    return defaultLibrariesLocation();
}


/**
 * @brief Get the location where libraries are stored by default.
 */
QString Application::defaultLibrariesLocation()
{
    QString result;
    result = QStandardPaths::writableLocation(
                QStandardPaths::AppLocalDataLocation);
    result =  QDir(result).absolutePath();
    QDir(result).mkpath(".");
    return result;
}


void Application::onLibrarySyncFinished(QString directory)
{
    Library lib(directory);
    if (lib.isValid()) {
        QScopedPointer<Synchronizer> sync(lib.createSynchronizer());
        if (sync) {
            sync->setLastSync(QDateTime::currentDateTime());
            sync->save();
        }
        auto dirs = directoriesWithRunningSync();
        dirs.removeAll(directory);
        setDirectoriesWithRunningSync(dirs);
    }
}


void Application::onLibrarySyncError(QString directory, QString error)
{
    auto errors = m_syncErrors.value(directory, QStringList()).toStringList();
    errors.append(error);
    m_syncErrors[directory] = errors;
    emit syncErrorsChanged();
}


template<typename T>
void Application::runSyncForLibrary(T library)
{
    if (library != nullptr && library->isValid()) {
        if (!m_directoriesWithRunningSync.contains(library->directory())) {
            QScopedPointer<Synchronizer> sync(library->createSynchronizer());
            if (!sync) {
                return;
            }
            auto key = sync->secretsKey();
            QString secret;
            if (!key.isEmpty()) {
                if (m_secrets.contains(key)) {
                    secret = m_secrets.value(key).toString();
                } else {
                    qCWarning(log) << "Missing sync secret for library"
                                           << library << library->name();
                    return;
                }
            }
            setDirectoriesWithRunningSync(
                        directoriesWithRunningSync() << library->directory());
            m_syncErrors.remove(library->directory());
            emit syncErrorsChanged();
            auto job = new SyncJob(library->directory(), secret);
            connect(qApp, &QCoreApplication::aboutToQuit, job, &SyncJob::stop);
            connect(job, &SyncJob::syncFinished,
                    this, &Application::onLibrarySyncFinished,
                    Qt::QueuedConnection);
            connect(job, &SyncJob::syncError,
                    this, &Application::onLibrarySyncError,
                    Qt::QueuedConnection);
            auto runner = new SyncRunner(job);
            QThreadPool::globalInstance()->start(runner);
        }
    }
}

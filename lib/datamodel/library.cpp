#include "library.h"

#include <QDebug>
#include <QDir>
#include <QQmlEngine>
#include <QtConcurrent>
#include <QTimer>

#include "application.h"
#include "libraryloader.h"
#include "toplevelitem.h"
#include "todolist.h"
#include "todo.h"
#include "task.h"
#include "sync/synchronizer.h"

#include "utils/jsonutils.h"

#include "utils/directorywatcher.h"


const QString Library::LibraryFileName = "library.json";

Q_LOGGING_CATEGORY(library, "net.rpdev.opentodolist.Library")

/**
   @brief Set the name of the library.
 */
Library::Library(QObject* parent) : QObject(parent),
    m_uid(QUuid::createUuid()),
    m_name(),
    m_directory(),
    m_topLevelItems(this),
    m_todos(this),
    m_tasks(this),
    m_directoryWatcher(new DirectoryWatcher(this)),
    m_loading(false),
    m_synchronizing(false),
    m_secretsMissing(false)
{
    auto timer = new QTimer(this);
    timer->setInterval(5000);
    timer->setSingleShot(true);
    connect(timer, &QTimer::timeout, this, &Library::load);
    connect(m_directoryWatcher, &DirectoryWatcher::directoryChanged, timer,
            static_cast<void(QTimer::*)()>(&QTimer::start));
    connect(&m_topLevelItems, &ItemContainer::countChanged,
            this, &Library::tagsChanged);
}

Library::Library(const QString& directory, QObject* parent) : Library(parent)
{
    m_directory = directory;
    m_directoryWatcher->setDirectory(directory);
}

Library::~Library()
{
}

/**
 * @brief Create a new note.
 *
 * This creates a new note in the library and returns a pointer to it.
 * Note that this method is intended to be called by QML. It is safe to
 * use the returned object in JavaScript/QML. When invoked from C++, do not
 * attempt to store the returned object for access over a longer time,
 * as the library might delete it at any time if need be and hence the pointer
 * might get dangling.
 */
Note *Library::addNote()
{
    NotePtr note;
    if (isValid()) {
        QDir dir(newItemLocation());
        dir.mkpath(".");
        note = NotePtr(new Note(dir));
    } else {
        note = NotePtr(new Note());
    }
    note->setWeight(m_topLevelItems.nextItemWeight());
    QQmlEngine::setObjectOwnership(note.data(), QQmlEngine::CppOwnership);
    appendItem(note);
    note->save();
    return note.data();
}

/**
 * @brief Create a new image.
 *
 * This creates a new image in the library and returns a pointer to it.
 * Note that this method is intended to be called by QML. It is safe to
 * use the returned object in JavaScript/QML. When invoked from C++, do not
 * attempt to store the returned object for access over a longer time,
 * as the library might delete it at any time if need be and hence the pointer
 * might get dangling.
 */
Image *Library::addImage()
{
    ImagePtr image;
    if (isValid()) {
        QDir dir(newItemLocation());
        dir.mkpath(".");
        image = ImagePtr(new Image(dir));
    } else {
        image = ImagePtr(new Image());
    }
    image->setWeight(m_topLevelItems.nextItemWeight());
    QQmlEngine::setObjectOwnership(image.data(), QQmlEngine::CppOwnership);
    appendItem(image);
    image->save();
    return image.data();
}

/**
 * @brief Create a new todo list.
 *
 * This creates a new todo list in the library and returns a pointer to it.
 * Note that this method is intended to be called by QML. It is safe to
 * use the returned object in JavaScript/QML. When invoked from C++, do not
 * attempt to store the returned object for access over a longer time,
 * as the library might delete it at any time if need be and hence the pointer
 * might get dangling.
 */
TodoList *Library::addTodoList()
{
    TodoListPtr todoList;
    if (isValid()) {
        QDir dir(newItemLocation());
        dir.mkpath(".");
        todoList = TodoListPtr(new TodoList(dir));
    } else {
        todoList = TodoListPtr(new TodoList());
    }
    todoList->m_library = this;
    todoList->setWeight(m_topLevelItems.nextItemWeight());
    QQmlEngine::setObjectOwnership(todoList.data(), QQmlEngine::CppOwnership);
    appendItem(todoList);
    todoList->save();
    return todoList.data();
}

bool Library::isValid() const
{
    return !m_directory.isEmpty() && QDir(m_directory).exists();
}

void Library::setName(const QString &name)
{
    if ( m_name != name ) {
        m_name = name;
        emit nameChanged();
        save();
    }
}

/**
 * @brief Remove the library from the application.
 *
 * This method removes the library from the application. Basically, it emits the libraryDeleted()
 * signal and then schedules the Library object for deletion. If @p deleteFiles is set to
 * true, the data on disk will be removed. Otherwise, the library files will be preserved and
 * hence can be restored later.
 */
void Library::deleteLibrary(bool deleteFiles)
{
    deleteLibrary(deleteFiles, std::function<void ()>());
}

/**
 * @brief Delete the library.
 *
 * This is an overloaded version of the deleteLibrary() method. It supports an
 * additional @p callback argument, which is a callable function taking no arguments
 * and which returns nothing. This function is called as soon as the actual deletion
 * process is done.
 *
 * @note The callback might either get called in the calling thread or in an
 * arbitrary helper thread. Do not make any assumptions where it gets called.
 */
void Library::deleteLibrary(bool deleteFiles, std::function<void ()> callback)
{
    if (m_synchronizing) {
        qCWarning(library) << "Cannot delete library" << this << this->name()
                           << ": A sync is currently running.";
        return;
    }
    emit deletingLibrary(this);
    QString directory = m_directory;
    m_directoryWatcher->setDirectory(QString());
    if (isValid() && deleteFiles) {
        QtConcurrent::run([=](){
            auto years = Library::years(directory);
            for (auto year : years) {
                auto months = Library::months(directory, year);
                for (auto month : months) {
                    QDir dir(directory + "/" + year + "/" + month);
                    for (auto entry : dir.entryList(QDir::Files)) {
                        if (!dir.remove(entry)) {
                            qCWarning(library) << "Failed to remove file" << entry << "from"
                                               << dir.absolutePath();
                        }
                    }
                    dir.cdUp();
                    if (!dir.rmdir(month)) {
                        qCWarning(library) << "Failed to remove" << dir.absoluteFilePath(month);
                    }
                }
                if (!QDir(directory).rmdir(year)) {
                    qCWarning(library) << "Failed to remove" << (directory + "/" + year);
                }
            }
            QDir dir(directory);
            for (auto entry : dir.entryList(QDir::Files | QDir::Hidden)) {
                if (!dir.remove(entry)) {
                    qCWarning(library) << "Failed to remove" << entry
                                       << "from" << dir.absolutePath();
                }
            }
            dir.cdUp();
            auto basename = QFileInfo(directory).baseName();
            if (!dir.rmdir(basename)) {
                qCWarning(library) << "Failed to remove library directory" << dir.absolutePath();
            }
            if (callback) {
                callback();
            }
        });
    } else {
        if (callback) {
            callback();
        }
    }
    emit libraryDeleted(this);
    deleteLater();
}

bool Library::load()
{
    bool result = false;
    if (isValid()) {
        QDir dir(m_directory);
        QString filename = dir.absoluteFilePath(LibraryFileName);
        bool ok;
        auto map = JsonUtils::loadMap(filename, &ok);
        if (ok) {
            fromMap(map);
            result = true;
        }
    }
    if (!m_loading && isValid()) {
        setLoading(true);
        LibraryLoader *loader = new LibraryLoader(this);
        loader->setDirectory(m_directory);
        connect(loader, &LibraryLoader::scanFinished, [=]() {
            setLoading(false);
            emit loadingFinished();
            loader->deleteLater();
        });
        connect(loader, &LibraryLoader::itemLoaded, this, &Library::appendItem);
        loader->scan();
    }
    return result;
}

bool Library::save()
{
    bool result = false;
    if (isValid()) {
        QDir dir(m_directory);
        QString filename = dir.absoluteFilePath(LibraryFileName);
        result = JsonUtils::patchJsonFile(filename, toMap());
    }
    return result;
}


/**
 * @brief Trigger a synchronization of the library.
 */
void Library::sync()
{
    if (isValid()) {
        auto sync = Synchronizer::fromDirectory(m_directory);
        if (sync != nullptr) {

        }
    }
}

ItemContainer* Library::topLevelItems()
{
    return &m_topLevelItems;
}

ItemContainer* Library::todos()
{
    return &m_todos;
}

ItemContainer* Library::tasks()
{
    return &m_tasks;
}

/**
 * @brief Get the location of a new item.
 *
 * Returns an absolute path where a new item can be stored. Returns an empty
 * string if the library is invalid.
 */
QString Library::newItemLocation() const
{
    QString result;
    if (isValid()) {
        QDir dir(m_directory);
        auto date = QDate::currentDate();
        auto subdir = QString("%1/%2");
        subdir = subdir.arg(date.year()).arg(date.month());
        result = dir.absoluteFilePath(subdir);
    }
    return result;
}

/**
 * @brief Get the list of year entries.
 *
 * This searches the @p directory for a list if directories potentially holding
 * item data of a library. Returns the list of entries (relative to the directory).
 */
QStringList Library::years(const QString& directory)
{
    QStringList result;
    QDir dir(directory);
    QRegExp re("^\\d{4}$");
    if (!directory.isEmpty() && dir.exists()) {
        for (auto entry : dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
            if (re.indexIn(entry) == 0) {
                result << entry;
            }
        }
    }
    return result;
}

QStringList Library::months(const QString& directory, const QString& year)
{
    QStringList result;
    if (!directory.isEmpty()) {
        auto path = directory + "/" + year;
        QDir dir(path);
        QRegExp re("^[1-9][0-2]?$");
        if (dir.exists()) {
            for (auto entry : dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
                if (re.indexIn(entry) == 0) {
                    result << entry;
                }
            }
        }
    }
    return result;
}


/**
 * @brief Whether the library is in the default libraries location.
 *
 * This property indicates if the library is located in the default libraries
 * location.
 */
bool Library::isInDefaultLocation() const
{
    bool result = false;
    if (isValid()) {
        auto defaultLibrariesLocation = Application::defaultLibrariesLocation();
        defaultLibrariesLocation = QDir::cleanPath(defaultLibrariesLocation);

        auto thisDir = QFileInfo(m_directory).absoluteDir().path();

        result = thisDir == defaultLibrariesLocation;
    }
    return result;
}

bool Library::loading() const
{
    return m_loading;
}

/**
 * @brief The UID of the library.
 */
QUuid Library::uid() const
{
    return m_uid;
}

/**
 * @brief Get the list of tags in the library.
 *
 * This returns the list of tags - sorted alphabetically - used within the library.
 */
QStringList Library::tags() const
{
    QSet<QString> tags;
    for (int i = 0; i < m_topLevelItems.count(); ++i) {
        auto item = qSharedPointerDynamicCast<TopLevelItem>(m_topLevelItems.item(i));
        for (auto tag : item->tags()) {
            tags.insert(tag);
        }
    }
    auto result = tags.values();
    result.sort(Qt::CaseInsensitive);
    return result;
}


/**
 * @brief Initialize the propertoes of the library from the JSON @p data.
 */
void Library::fromJson(const QByteArray data)
{
    auto doc = QJsonDocument::fromJson(data);
    if (doc.isObject()) {
        fromMap(doc.toVariant().toMap());
    }
}


/**
 * @brief Indicates if the library has a synchronizer.
 *
 * This property indicates if the library has a synchronizer set up.
 */
bool Library::hasSynchronizer() const
{
    bool result = false;
    if (isValid()) {
        auto sync = createSynchronizer();
        if (sync != nullptr) {
            result = true;
            delete sync;
        }
    }
    return result;
}


/**
 * @brief Indicates if the library is currently synchronizing.
 *
 * This property indicates if the library currently is
 * synchronizing, i.e. a background job is running which
 * synchronizes the local files of the library with some
 * storage server.
 */
bool Library::synchronizing() const
{
    return m_synchronizing;
}


/**
 * @brief Set the synchronzing status of the library.
 */
void Library::setSynchronizing(bool synchronizing)
{
    if (m_synchronizing != synchronizing) {
        m_synchronizing = synchronizing;
        emit synchronizingChanged();
    }
}


/**
 * @brief Create a synchronizer for the library.
 *
 * This creates an instance of the Synchronizer class (or more precise, a
 * concrete synchronizer depending on what the user set up for the library)
 * and returns it. The ownership of the object is with the caller.
 *
 * If specified, the synchronizer will be owned by the @p parent.
 *
 * If no synchronizer has been set up for the library, a nullptr is returned.
 *
 * @note The method also returns nullptrs if the library is invalid, i.e.
 * it has no local directory associated with it.
 */
Synchronizer *Library::createSynchronizer(QObject *parent) const
{
    if (isValid()) {
        return Synchronizer::fromDirectory(directory(), parent);
    } else {
        return nullptr;
    }
}


/**
 * @brief Indicates that secrets for the library are missing.
 *
 * This is a helper property which is used on the GUI domain only. It is
 * set by the application to flag the library as having missing login
 * credentials. This can happen if e.g. the user removed the credentials
 * from the platform specific credential store or some other error occurred
 * which causes the app from failing to re-read credentials.
 *
 * @sa setSecretsMissing
 */
bool Library::secretsMissing() const
{
    return m_secretsMissing;
}


/**
 * @brief Flag the libary as not having secrets required for synchronization.
 */
void Library::setSecretsMissing(bool secretsMissing)
{
    if (m_secretsMissing != secretsMissing) {
        m_secretsMissing = secretsMissing;
        emit secretsMissingChanged();
    }
}


/**
 * @brief Set the UID of the library.
 */
void Library::setUid(const QUuid& uid)
{
    if (m_uid != uid) {
        m_uid = uid;
        emit uidChanged();
    }
}

void Library::setLoading(bool loading)
{
    if (m_loading != loading) {
        m_loading = loading;
        emit loadingChanged();
    }
}

QVariantMap Library::toMap() const
{
    QVariantMap result;
    result["uid"] = m_uid;
    result["name"] = m_name;
    return result;
}

void Library::fromMap(QVariantMap map)
{
    setUid(map.value("uid", m_uid).toUuid());
    setName(map.value("name", m_name).toString());
}

void Library::appendItem(ItemPtr item)
{
    auto topLevelItem = qSharedPointerDynamicCast<TopLevelItem>(item);
    if (!topLevelItem.isNull()) {
        auto todoList = qSharedPointerDynamicCast<TodoList>(item);
        if (todoList) {
            todoList->m_library = this;
        }
        m_topLevelItems.updateOrInsert(item);
        connect(topLevelItem.data(), &TopLevelItem::tagsChanged, this, &Library::tagsChanged);
    } else {
        auto todo = qSharedPointerDynamicCast<Todo>(item);
        if (!todo.isNull()) {
            todo->setLibrary(this);
            m_todos.updateOrInsert(item);
        } else {
            auto task = qSharedPointerDynamicCast<Task>(item);
            if (!task.isNull()) {
                m_tasks.updateOrInsert(item);
            }
        }
    }
}

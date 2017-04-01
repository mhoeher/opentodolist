#include "library.h"

#include <QDebug>
#include <QDir>
#include <QJsonDocument>
#include <QQmlEngine>
#include <QtConcurrent>
#include <QTimer>

#include "libraryloader.h"
#include "toplevelitem.h"
#include "todo.h"
#include "task.h"

#include "utils/directorywatcher.h"


const QString Library::LibraryFileName = "library.json";

Q_LOGGING_CATEGORY(library, "net.rpdev.opentodolist.Library");

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
    m_loading(false)
{
    auto timer = new QTimer(this);
    timer->setInterval(5000);
    timer->setSingleShot(true);
    connect(timer, &QTimer::timeout, this, &Library::load);
    connect(m_directoryWatcher, &DirectoryWatcher::directoryChanged, timer,
            static_cast<void(QTimer::*)()>(&QTimer::start));
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
            if (!dir.remove(Library::LibraryFileName)) {
                qCWarning(library) << "Failed to remove" << Library::LibraryFileName
                                   << "from" << dir.absolutePath();
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
        callback();
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
        QFile file(filename);
        if (file.exists()) {
            if (file.open(QIODevice::ReadWrite)) {
                QJsonParseError error;
                auto map = QJsonDocument::fromJson(file.readAll(), &error).toVariant().toMap();
                if (error.error == QJsonParseError::NoError) {
                    fromMap(map);
                    result = true;
                } else {
                    qWarning() << "Failed to parse file" << filename <<":" << error.errorString();
                }
                file.close();

            } else {
                qWarning() << "Failed to open file" << filename << "for reading:"
                           << file.errorString();
            }
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
        QFile file(filename);
        QVariantMap map;
        if (file.exists()) {
            if (file.open(QIODevice::ReadOnly)) {
                QJsonParseError error;
                map = QJsonDocument::fromJson(file.readAll(), &error).toVariant().toMap();
                if (error.error != QJsonParseError::NoError) {
                    qWarning() << "Failed to parse" << filename << ":" << error.errorString();
                }
                file.close();
            } else {
                qWarning() << "Failed to open" << filename << "for reading:" << file.errorString();
            }
        }
        auto newMap = toMap();
        for (auto key : newMap.keys()) {
            map[key] = newMap[key];
        }
        if (file.open(QIODevice::WriteOnly)) {
            auto data = QJsonDocument::fromVariant(map).toJson(QJsonDocument::Indented);
            file.write(data);
            result = true;
            file.close();
        } else {
            qWarning() << "Failed to open" << filename << "for writing:" << file.errorString();
        }
    }
    return result;
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
    result.sort();
    return result;
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
        m_topLevelItems.updateOrInsert(item);
        connect(topLevelItem.data(), &TopLevelItem::tagsChanged, this, &Library::tagsChanged);
        emit tagsChanged();
    } else {
        auto todo = qSharedPointerDynamicCast<Todo>(item);
        if (!todo.isNull()) {
            m_todos.updateOrInsert(item);
        } else {
            auto task = qSharedPointerDynamicCast<Task>(item);
            if (!task.isNull()) {
                m_tasks.updateOrInsert(item);
            }
        }
    }
}

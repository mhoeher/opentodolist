#include "library.h"

#include <QDebug>
#include <QDir>
#include <QJsonDocument>
#include <QQmlEngine>
#include <QtConcurrent>

#include "libraryloader.h"
#include "todo.h"
#include "task.h"


const QString Library::LibraryFileName = "library.json";


/**
   @brief Set the name of the library.
 */
Library::Library(QObject* parent) : QObject(parent),
    m_name(),
    m_directory(),
    m_topLevelItems(),
    m_todos(),
    m_tasks(),
    m_loading(false)
{

}

Library::Library(const QString& directory, QObject* parent) : Library(parent)
{
    m_directory = directory;
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
    NotePtr note(new Note(QDir(m_directory)));
    QQmlEngine::setObjectOwnership(note.data(), QQmlEngine::CppOwnership);
    m_topLevelItems.addItem(note);
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
    ImagePtr image(new Image(QDir(m_directory)));
    QQmlEngine::setObjectOwnership(image.data(), QQmlEngine::CppOwnership);
    m_topLevelItems.addItem(image);
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
    TodoListPtr todoList(new TodoList(QDir(m_directory)));
    todoList->m_library = this;
    QQmlEngine::setObjectOwnership(todoList.data(), QQmlEngine::CppOwnership);
    m_topLevelItems.addItem(todoList);
    todoList->save();
    return todoList.data();
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
 * signal and then schedules the Library object for deletion.
 */
void Library::deleteLibrary()
{
    QString directory;
    QtConcurrent::run([=](){
        QDir dir(directory);
        for (auto entry : dir.entryList(QDir::Files)) {
            dir.remove(entry);
        }
        dir.remove(".");
    });
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
        m_loading = true;
        LibraryLoader *loader = new LibraryLoader(this);
        loader->setDirectory(m_directory);
        connect(loader, &LibraryLoader::scanFinished, [=]() {
           m_loading = false;
           emit loadingFinished();
           loader->deleteLater();
        });
        connect(loader, &LibraryLoader::itemLoaded, [=](ItemPtr item) {
            auto topLevelItem = qSharedPointerDynamicCast<TodoListPtr>(item);
            if (!topLevelItem.isNull()) {
                m_topLevelItems.updateOrInsert(item);
            } else {
                auto todo = qSharedPointerDynamicCast<TodoPtr>(item);
                if (!todo.isNull()) {
                    m_todos.updateOrInsert(item);
                } else {
                    auto task = qSharedPointerDynamicCast<TaskPtr>(item);
                    if (!task.isNull()) {
                        m_tasks.updateOrInsert(item);
                    }
                }
            }
        });
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

ItemContainer& Library::topLevelItems()
{
    return m_topLevelItems;
}

ItemContainer& Library::todos()
{
    return m_todos;
}

ItemContainer& Library::tasks()
{
    return m_tasks;
}

QVariantMap Library::toMap() const
{
    QVariantMap result;
    result["name"] = m_name;
    return result;
}

void Library::fromMap(QVariantMap map)
{
    setName(map.value("name", m_name).toString());
}

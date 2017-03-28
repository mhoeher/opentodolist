#include "migrator_2_x_to_3_x.h"

#include "application.h"
#include "image.h"
#include "note.h"
#include "task.h"
#include "todo.h"
#include "todolist.h"

#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QJsonDocument>
#include <QtConcurrent>


static QVariantMap load_file(const QString &filename) {
    QFile file(filename);
    if (file.open(QIODevice::ReadOnly)) {
        QJsonParseError error;
        auto doc = QJsonDocument::fromJson(file.readAll(), &error);
        if (error.error == QJsonParseError::NoError) {
            auto result = doc.toVariant().toMap();
            QFileInfo fi(filename);
            if (fi.dir().exists("notes.html")) {
                QFile notesFile(fi.dir().absoluteFilePath("notes.html"));
                if (notesFile.open(QIODevice::ReadOnly)) {
                    result["notes"] = QString(notesFile.readAll());
                }
            }
            return result;
        }
    }
    return QVariantMap();
}

static void migrate_note(const QString directory, const QFileInfo fi) {
    auto data = load_file(fi.absoluteFilePath());
    QDir dir(directory);
    Note note(dir);
    note.setUid(data.value("uid", note.uid()).toUuid());
    note.setTitle(data.value("title", note.title()).toString());
    note.setTags(data.value("tags").toStringList());
    note.setColor(data.value("colorName").toString());
    note.setNotes(data.value("notes").toString());
}

static void migrate_image(const QString directory, const QFileInfo fi) {
    auto data = load_file(fi.absoluteFilePath());
    QDir dir(directory);
    Image image(dir);
    image.setUid(data.value("uid", image.uid()).toUuid());
    image.setTitle(data.value("title", image.title()).toString());
    image.setTags(data.value("tags").toStringList());
    image.setColor(data.value("colorName").toString());
    image.setImage(fi.dir().absoluteFilePath(data.value("image").toString()));
    image.setNotes(data.value("notes").toString());
}

static void migrate_task(const QString &directory, Todo& todo,
                         const QFileInfo fi) {
    auto data = load_file(fi.absoluteFilePath());
    QDir dir(directory);
    Task task(dir);
    task.setUid(data.value("uid", task.uid()).toUuid());
    task.setTitle(data.value("title", task.title()).toString());
    task.setDone(data.value("done", task.done()).toBool());
    task.setTodoUid(todo.uid());
}

static void migrate_todo(const QString directory, TodoList& todoList,
                         const QFileInfo fi) {
    auto data = load_file(fi.absoluteFilePath());
    QDir dir(directory);
    Todo todo(dir);
    todo.setUid(data.value("uid", todo.uid()).toUuid());
    todo.setTitle(data.value("title", todo.title()).toString());
    todo.setDone(data.value("done", todo.done()).toBool());
    todo.setTodoListUid(todoList.uid());
    todo.setNotes(data.value("notes").toString());
    QDir tasks(fi.dir().absoluteFilePath("tasks"));
    for (auto taskDir : tasks.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        QFileInfo taskFile(taskDir + "/task.opentodolist");
        if (taskFile.exists()) {
            migrate_task(directory, todo, taskFile);
        }
    }
}

static void migrate_todolist(const QString directory, const QFileInfo fi) {
    auto data = load_file(fi.absoluteFilePath());
    QDir dir(directory);
    TodoList todoList(dir);
    todoList.setUid(data.value("uid", todoList.uid()).toUuid());
    todoList.setTitle(data.value("title", todoList.title()).toString());
    todoList.setTags(data.value("tags").toStringList());
    todoList.setColor(data.value("colorName").toString());
    todoList.setNotes(data.value("notes").toString());
    QDir todos(fi.dir().absoluteFilePath("todos"));
    for (auto todoDir : todos.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        QFileInfo todoFile(todoDir + "/todo.opentodolist");
        if (todoFile.exists()) {
            migrate_todo(directory, todoList, todoFile);
        }
    }
}

static void migrate_to_3_0(const QString directory) {
    QDirIterator it(directory, {"note.opentodolist",
                                "image.opentodolist",
                                "todolist.opentodolist"},
                    QDir::NoDotAndDotDot | QDir::Files,
                    QDirIterator::Subdirectories);
    while (it.hasNext()) {
        it.next();
        auto fi = it.fileInfo();
        auto basename = fi.baseName();
        if (basename == "note") {
            migrate_note(directory, fi);
        } else if (basename == "image") {
            migrate_image(directory, fi);
        } else if (basename == "todolist") {
            migrate_todolist(directory, fi);
        }
    }
}

Migrator_2_x_to_3_x::Migrator_2_x_to_3_x(QObject *parent) : QObject(parent)
{
}

void Migrator_2_x_to_3_x::run(Application* application)
{
    auto settings = application->m_settings;
    auto oldLibraries = settings->beginReadArray("Library");
    for (int i = 0; i < oldLibraries; ++i) {
        settings->setArrayIndex(i);
        QString name = settings->value("name").toString();
        QString type = settings->value("type").toString();
        QString directory = settings->value("directory").toString() + "/OpenTodoList";
        auto library = new Library(directory, application);
        library->setName(name);
        application->appendLibrary(library);
        QtConcurrent::run([=]() {
            migrate_to_3_0(directory);
        });
    }
    settings->endArray();
}

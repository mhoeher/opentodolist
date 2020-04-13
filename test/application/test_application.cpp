#include <QObject>
#include <QSharedPointer>
#include <QSignalSpy>
#include <QTemporaryDir>
#include <QTest>

#include "application.h"
#include "datamodel/library.h"
#include "datamodel/note.h"
#include "datamodel/image.h"
#include "datamodel/todolist.h"
#include "datamodel/todo.h"
#include "datamodel/task.h"
#include "models/itemsmodel.h"
#include "models/librariesmodel.h"

class ApplicationTest : public QObject
{
    Q_OBJECT

    QTemporaryDir *m_appDir;
    Application *m_app;

private slots:
    void init();
    void testLibraryHandling();
    void cleanup();
};

void ApplicationTest::init()
{
    m_appDir = new QTemporaryDir();
    QVERIFY2(m_appDir->isValid(), "Failed to create temporary directory.");
    m_app = new Application(m_appDir->path());
}

void ApplicationTest::testLibraryHandling()
{
    LibrariesModel libraries;
    libraries.setCache(m_app->cache());

    QSharedPointer<Library> lib(m_app->addLibrary(QVariantMap({ { "Name", "A library" } })));
    QVERIFY(lib != nullptr);

    {
        QSignalSpy rowsAdded(&libraries, &LibrariesModel::rowsInserted);
        QVERIFY(rowsAdded.wait());
        QCOMPARE(rowsAdded.count(), 1);
    }

    ItemsModel topLevelItems;
    topLevelItems.setCache(m_app->cache());
    topLevelItems.setParentItem(lib->uid());

    QSharedPointer<TodoList> todoList(
            m_app->addTodoList(lib.data(), QVariantMap({ { "title", "A todo list" } })));
    {
        QSignalSpy rowsAdded(&topLevelItems, &ItemsModel::rowsInserted);
        QVERIFY(rowsAdded.wait());
        QCOMPARE(topLevelItems.count(), 1);
        QCOMPARE(topLevelItems.index(0).data(ItemsModel::ItemRole).value<Item *>()->uid(),
                 todoList->uid());
        QCOMPARE(topLevelItems.index(0).data(ItemsModel::ItemRole).value<Item *>()->title(),
                 todoList->title());
        QCOMPARE(topLevelItems.index(0).data(ItemsModel::ItemRole).value<Item *>()->itemType(),
                 todoList->itemType());
    }

    ItemsModel todos;
    todos.setCache(m_app->cache());
    todos.setParentItem(todoList->uid());

    QSharedPointer<Todo> todo(
            m_app->addTodo(lib.data(), todoList.data(), QVariantMap({ { "title", "A todo" } })));
    {
        QSignalSpy rowsAdded(&todos, &ItemsModel::rowsInserted);
        QVERIFY(rowsAdded.wait());
        QCOMPARE(todos.count(), 1);
        QCOMPARE(todos.index(0).data(ItemsModel::ItemRole).value<Item *>()->uid(), todo->uid());
        QCOMPARE(todos.index(0).data(ItemsModel::ItemRole).value<Item *>()->title(), todo->title());
        QCOMPARE(todos.index(0).data(ItemsModel::ItemRole).value<Item *>()->itemType(),
                 todo->itemType());
    }

    ItemsModel tasks;
    tasks.setCache(m_app->cache());
    tasks.setParentItem(todo->uid());

    QSharedPointer<Task> task(
            m_app->addTask(lib.data(), todo.data(), QVariantMap({ { "title", "A task" } })));
    {
        QSignalSpy rowsAdded(&tasks, &ItemsModel::rowsInserted);
        QVERIFY(rowsAdded.wait());
        QCOMPARE(tasks.count(), 1);
        QCOMPARE(tasks.index(0).data(ItemsModel::ItemRole).value<Item *>()->uid(), task->uid());
        QCOMPARE(tasks.index(0).data(ItemsModel::ItemRole).value<Item *>()->title(), task->title());
        QCOMPARE(tasks.index(0).data(ItemsModel::ItemRole).value<Item *>()->itemType(),
                 task->itemType());
    }

    QSharedPointer<Note> note(m_app->addNote(lib.data(), QVariantMap({ { "title", "A note" } })));
    {
        QSignalSpy rowsAdded(&topLevelItems, &ItemsModel::rowsInserted);
        QVERIFY(rowsAdded.wait());
        QCOMPARE(topLevelItems.count(), 2);
        QCOMPARE(topLevelItems.index(1).data(ItemsModel::ItemRole).value<Item *>()->uid(),
                 note->uid());
        QCOMPARE(topLevelItems.index(1).data(ItemsModel::ItemRole).value<Item *>()->title(),
                 note->title());
        QCOMPARE(topLevelItems.index(1).data(ItemsModel::ItemRole).value<Item *>()->itemType(),
                 note->itemType());
    }

    QSharedPointer<Image> image(
            m_app->addImage(lib.data(), QVariantMap({ { "title", "An image" } })));
    {
        QSignalSpy rowsAdded(&topLevelItems, &ItemsModel::rowsInserted);
        QVERIFY(rowsAdded.wait());
        QCOMPARE(topLevelItems.count(), 3);
        QCOMPARE(topLevelItems.index(2).data(ItemsModel::ItemRole).value<Item *>()->uid(),
                 image->uid());
        QCOMPARE(topLevelItems.index(2).data(ItemsModel::ItemRole).value<Item *>()->title(),
                 image->title());
        QCOMPARE(topLevelItems.index(2).data(ItemsModel::ItemRole).value<Item *>()->itemType(),
                 image->itemType());
    }
}

void ApplicationTest::cleanup()
{
    delete m_app;
    delete m_appDir;
}

QTEST_MAIN(ApplicationTest)
#include "test_application.moc"

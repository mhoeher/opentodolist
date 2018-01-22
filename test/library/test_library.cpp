#include "application.h"
#include "datamodel/image.h"
#include "datamodel/library.h"
#include "datamodel/note.h"
#include "datamodel/todolist.h"
#include "datamodel/todo.h"
#include "datamodel/task.h"

#include <QObject>
#include <QQmlEngine>
#include <QSet>
#include <QSettings>
#include <QSignalSpy>
#include <QTemporaryDir>
#include <QTest>


#include <iostream>

class LibraryTest : public QObject
{

    Q_OBJECT

private slots:

    void init();
    void testProperties();
    void testPersistence();
    void testAddNote();
    void testAddImage();
    void testAddTodoList();
    void testAddTodo();
    void addTask();
    void testTags();
    void testLoad();
    void testDeleteLibrary();
    void testFromJson();
    void cleanup();

private:

    QTemporaryDir *m_dir;

};


void LibraryTest::init()
{
    m_dir = new QTemporaryDir();
}

void LibraryTest::testProperties()
{
    Library lib;
    QVERIFY(!lib.isValid());

    QSignalSpy nameChanged(&lib, &Library::nameChanged);

    lib.setName("My Library");

    QCOMPARE(nameChanged.count(), 1);

    QCOMPARE(lib.name(), QString("My Library"));
}

void LibraryTest::testPersistence()
{
    Library lib(m_dir->path()), anotherLib(m_dir->path());
    QVERIFY(lib.isValid());

    QSignalSpy nameChanged(&anotherLib, &Library::nameChanged);
    QSignalSpy uidChanged(&anotherLib, &Library::uidChanged);

    lib.setName("My Library");

    anotherLib.load();

    QCOMPARE(nameChanged.count(), 1);
    QCOMPARE(uidChanged.count(), 1);

    QCOMPARE(anotherLib.name(), QString("My Library"));
    QCOMPARE(anotherLib.uid(), lib.uid());
}

void LibraryTest::testAddNote()
{
    Library lib;
    auto note = lib.addNote();
    QVERIFY(note != nullptr);
    QCOMPARE(QQmlEngine::objectOwnership(note), QQmlEngine::CppOwnership);
}

void LibraryTest::testAddImage()
{
    Library lib;
    auto image = lib.addImage();
    QVERIFY(image != nullptr);
    QCOMPARE(QQmlEngine::objectOwnership(image), QQmlEngine::CppOwnership);
}

void LibraryTest::testAddTodoList()
{
    Library lib;
    auto list = lib.addTodoList();
    QVERIFY(list != nullptr);
    QCOMPARE(QQmlEngine::objectOwnership(list), QQmlEngine::CppOwnership);
}

void LibraryTest::testAddTodo()
{
    Library lib;
    auto list = lib.addTodoList();
    auto todo = list->addTodo();

    QVERIFY(todo != nullptr);
    QCOMPARE(QQmlEngine::objectOwnership(todo), QQmlEngine::CppOwnership);
    QCOMPARE(todo->todoListUid(), list->uid());
}

void LibraryTest::addTask()
{
    Library lib;
    {
        auto list = lib.addTodoList();

        auto todo = list->addTodo();
        QVERIFY(todo != nullptr);

        auto task = todo->addTask();

        QVERIFY(task != nullptr);
        QCOMPARE(QQmlEngine::objectOwnership(task), QQmlEngine::CppOwnership);
        QCOMPARE(task->todoUid(), todo->uid());
    }
}

void LibraryTest::testTags()
{
    Library lib;
    auto note1 = lib.addNote();
    auto note2 = lib.addNote();

    // Wait for addition to finish:
    while (lib.topLevelItems()->count() < 2) {
        QThread::msleep(10);
    }

    QCOMPARE(lib.tags(), QStringList());

    QSignalSpy tagsChanged(&lib, &Library::tagsChanged);

    note1->addTag("Foo");
    QCOMPARE(tagsChanged.count(), 1);
    QCOMPARE(lib.tags(), QStringList({"Foo"}));

    note2->addTag("Foo");
    note2->addTag("Bar");
    QCOMPARE(tagsChanged.count(), 3);
    QCOMPARE(lib.tags(), QStringList({"Bar", "Foo"}));

    note2->setTags({});
    QCOMPARE(tagsChanged.count(), 4);
    QCOMPARE(lib.tags(), QStringList({"Foo"}));
}

void LibraryTest::testLoad()
{
    Library lib(m_dir->path());
    lib.save();
    lib.addNote();
    lib.addImage();
    auto todoList = lib.addTodoList();
    auto todo = todoList->addTodo();
    todo->addTask();

    Library lib2(m_dir->path());
    QSignalSpy loadingFinished(&lib2, &Library::loadingFinished);
    QVERIFY(lib2.load());
    QVERIFY(loadingFinished.wait(10000));
    QThread::sleep(1);
    QSet<QUuid> uuids;
    QSet<QString> itemTypes;
    QSet<QString> files;
    QCOMPARE(lib2.topLevelItems()->count(), 3);
    QCOMPARE(lib2.todos()->count(), 1);
    QCOMPARE(lib2.tasks()->count(), 1);
    uuids.insert(lib2.topLevelItems()->item(0)->uid());
    uuids.insert(lib2.topLevelItems()->item(1)->uid());
    uuids.insert(lib2.topLevelItems()->item(2)->uid());
    uuids.insert(lib2.todos()->item(0)->uid());
    uuids.insert(lib2.tasks()->item(0)->uid());
    itemTypes.insert(lib2.topLevelItems()->item(0)->itemType());
    itemTypes.insert(lib2.topLevelItems()->item(1)->itemType());
    itemTypes.insert(lib2.topLevelItems()->item(2)->itemType());
    itemTypes.insert(lib2.todos()->item(0)->itemType());
    itemTypes.insert(lib2.tasks()->item(0)->itemType());
    files.insert(lib2.topLevelItems()->item(0)->filename());
    files.insert(lib2.topLevelItems()->item(1)->filename());
    files.insert(lib2.topLevelItems()->item(2)->filename());
    files.insert(lib2.todos()->item(0)->filename());
    files.insert(lib2.tasks()->item(0)->filename());
    QCOMPARE(uuids.count(), 5);
    QCOMPARE(itemTypes.count(), 5);
    QCOMPARE(files.count(), 5);
}

void LibraryTest::testDeleteLibrary()
{
    QDir dir(m_dir->path() + "/Library");
    QVERIFY(dir.mkpath("."));
    auto lib = new Library(dir.absolutePath());
    lib->save();
    {
        QVERIFY(lib->addNote() != nullptr);
        QVERIFY(lib->addImage() != nullptr);
        auto todoList = lib->addTodoList();
        QVERIFY(todoList != nullptr);
        auto todo = todoList->addTodo();
        QVERIFY(todo != nullptr);
        QVERIFY(todo->addTask() != nullptr);
        QThread::sleep(1); // to prevent warnings
    }

    bool called = false;
    int countdown = 10;
    lib->deleteLibrary(false, [&]() { called = true; });
    while (!called) {
        --countdown;
        QThread::sleep(1);
    }
    QVERIFY(called);
    QVERIFY(countdown > 0);

    // We wanted to preserve files, make sure directory is still there:
    dir.refresh();
    QVERIFY(dir.exists());


    lib = new Library(dir.absolutePath());
    QThread::sleep(1);

    // Delete also files on disk:
    called = false;
    countdown = 10;
    lib->deleteLibrary(true, [&]() { called = true; });
    while (!called) {
        --countdown;
        QThread::sleep(1);
    }
    dir.refresh();
    QVERIFY(!dir.exists());
}

void LibraryTest::testFromJson()
{
    Library lib;
    lib.fromJson("{"
                 "\"name\": \"foo\","
                 "\"uid\": \"{6ca12b27-fc18-4257-9460-ef8dfed622bb}\""
                 "}"
                );
    QCOMPARE(lib.uid(), QUuid("{6ca12b27-fc18-4257-9460-ef8dfed622bb}"));
    QCOMPARE(lib.name(), QString("foo"));
}

void LibraryTest::cleanup()
{
    delete m_dir;
}

QTEST_MAIN(LibraryTest)
#include "test_library.moc"

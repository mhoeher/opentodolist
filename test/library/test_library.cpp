#include "application.h"
#include "image.h"
#include "library.h"
#include "note.h"
#include "todolist.h"
#include "todo.h"
#include "task.h"

#include <QObject>
#include <QQmlEngine>
#include <QSet>
#include <QSettings>
#include <QSignalSpy>
#include <QTemporaryDir>
#include <QTest>

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
    void testLoad();
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

    lib.setName("My Library");

    anotherLib.load();

    QCOMPARE(nameChanged.count(), 1);

    QCOMPARE(anotherLib.name(), QString("My Library"));
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
    QCOMPARE(lib.topLevelItems()->count(), 3);
    QCOMPARE(lib.todos()->count(), 1);
    QCOMPARE(lib.tasks()->count(), 1);
    uuids.insert(lib.topLevelItems()->item(0)->uid());
    uuids.insert(lib.topLevelItems()->item(1)->uid());
    uuids.insert(lib.topLevelItems()->item(2)->uid());
    uuids.insert(lib.todos()->item(0)->uid());
    uuids.insert(lib.tasks()->item(0)->uid());
    itemTypes.insert(lib.topLevelItems()->item(0)->itemType());
    itemTypes.insert(lib.topLevelItems()->item(1)->itemType());
    itemTypes.insert(lib.topLevelItems()->item(2)->itemType());
    itemTypes.insert(lib.todos()->item(0)->itemType());
    itemTypes.insert(lib.tasks()->item(0)->itemType());
    QCOMPARE(uuids.count(), 5);
    QCOMPARE(itemTypes.count(), 5);
}

void LibraryTest::cleanup()
{
    delete m_dir;
}

QTEST_MAIN(LibraryTest)
#include "test_library.moc"

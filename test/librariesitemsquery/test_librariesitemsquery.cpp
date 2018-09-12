#include <QObject>
#include <QRegularExpression>
#include <QSignalSpy>
#include <QTemporaryDir>
#include <QTest>

#include "datamodel/image.h"
#include "datamodel/library.h"
#include "datamodel/note.h"
#include "datamodel/task.h"
#include "datamodel/todo.h"
#include "datamodel/todolist.h"
#include "datastorage/cache.h"
#include "datastorage/insertorupdateitemsquery.h"
#include "datastorage/librariesitemsquery.h"


class LibrariesItemsQueryTest : public QObject
{
  Q_OBJECT

private slots:

  void initTestCase() {}
  void init() {}
  void run();
  void cleanup() {}
  void cleanupTestCase() {}
};




void LibrariesItemsQueryTest::run()
{
    QTest::ignoreMessage(QtDebugMsg,
                         QRegularExpression(".*Cache is uninitialized.*"));
    QTemporaryDir tmpDir;
    Cache cache;
    cache.setCacheDirectory(tmpDir.path());
    cache.setCacheSize(1024*1024);
    QVERIFY(cache.open());

    {
        auto q = new LibrariesItemsQuery();
        QSignalSpy librariesAvailable(
                    q, &LibrariesItemsQuery::librariesAvailable);
        QSignalSpy destroyed(q, &LibrariesItemsQuery::destroyed);
        cache.run(q);
        QVERIFY(destroyed.wait());
        QCOMPARE(librariesAvailable.count(), 1);
        QCOMPARE(librariesAvailable.at(0).at(0).toList().count(), 0);
    }

    Library lib;
    auto todoList = lib.addTodoList();
    todoList->setTitle("A todo list");
    auto todo = todoList->addTodo();
    todo->setTitle("A todo");
    auto task = todo->addTask();
    task->setTitle("A task");
    auto note = lib.addNote();
    note->setTitle("A note");
    auto image = lib.addImage();
    image->setTitle("An image");

    {
        auto q = new InsertOrUpdateItemsQuery;
        QSignalSpy finished(q, &InsertOrUpdateItemsQuery::finished);
        QSignalSpy destroyed(q, &InsertOrUpdateItemsQuery::destroyed);

        q->add(&lib);
        q->add(todoList);
        q->add(todo);
        q->add(task);
        q->add(note);
        q->add(image);
        cache.run(q);

        QVERIFY(destroyed.wait());
        QCOMPARE(finished.count(), 1);
    }

    {
        auto q = new LibrariesItemsQuery();
        QSignalSpy librariesAvailable(
                    q, &LibrariesItemsQuery::librariesAvailable);
        QSignalSpy destroyed(q, &LibrariesItemsQuery::destroyed);
        cache.run(q);
        QVERIFY(destroyed.wait());
        QCOMPARE(librariesAvailable.count(), 1);
        auto libraries = librariesAvailable.at(0).at(0).toList();
        QCOMPARE(libraries.count(), 1);
        auto entry = libraries.at(0).value<LibraryCacheEntry>();
        QVERIFY(entry.valid);
        auto eEntry = lib.encache();
        QCOMPARE(entry, lib.encache());
    }

    {
        lib.setName("Changed library title");
        todoList->setTitle("Changed todo list");
        todo->setTitle("Changed todo");
        task->setTitle("Changed task");
        note->setTitle("Changed note");
        image->setTitle("Changed image");

        auto q = new InsertOrUpdateItemsQuery;
        QSignalSpy finished(q, &InsertOrUpdateItemsQuery::finished);
        QSignalSpy destroyed(q, &InsertOrUpdateItemsQuery::destroyed);

        q->add(&lib);
        q->add(todoList);
        q->add(todo);
        q->add(task);
        q->add(note);
        q->add(image);
        cache.run(q);

        QVERIFY(destroyed.wait());
        QCOMPARE(finished.count(), 1);
    }

    {
        auto q = new LibrariesItemsQuery();
        QSignalSpy librariesAvailable(
                    q, &LibrariesItemsQuery::librariesAvailable);
        QSignalSpy destroyed(q, &LibrariesItemsQuery::destroyed);
        cache.run(q);
        QVERIFY(destroyed.wait());
        QCOMPARE(librariesAvailable.count(), 1);
        auto libraries = librariesAvailable.at(0).at(0).toList();
        QCOMPARE(libraries.count(), 1);
        auto entry = libraries.at(0).value<LibraryCacheEntry>();
        QVERIFY(entry.valid);
        auto eEntry = lib.encache();
        QCOMPARE(entry, lib.encache());
    }
}

QTEST_MAIN(LibrariesItemsQueryTest)
#include "test_librariesitemsquery.moc"

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

class InsertOrUpdateItemsQueryTest : public QObject
{
  Q_OBJECT

private slots:

  void initTestCase() {}
  void init() {}
  void insertOrUpdate();
  void cleanup() {}
  void cleanupTestCase() {}
};




void InsertOrUpdateItemsQueryTest::insertOrUpdate()
{
//    QTest::ignoreMessage(QtDebugMsg,
//                         QRegularExpression(".*Cache is uninitialized.*"));
    QTemporaryDir tmpDir;
    Cache cache;
    cache.setCacheDirectory(tmpDir.path());
    cache.setCacheSize(1024*1024);
    QVERIFY(cache.open());

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
        QSignalSpy cacheFinished(&cache, &Cache::finished);
        QSignalSpy dataChanged(&cache, &Cache::dataChanged);
        QSignalSpy destroyed(q, &InsertOrUpdateItemsQuery::destroyed);

        q->add(&lib);
        q->add(todoList);
        q->add(todo);
        q->add(task);
        q->add(note);
        q->add(image);
        cache.run(q);

        QVERIFY(cacheFinished.wait());
        QCOMPARE(destroyed.count(), 1);
        QCOMPARE(finished.count(), 1);
        QCOMPARE(dataChanged.count(), 1);
    }

    {
        auto q = new InsertOrUpdateItemsQuery;
        QSignalSpy finished(q, &InsertOrUpdateItemsQuery::finished);
        QSignalSpy cacheFinished(&cache, &Cache::finished);
        QSignalSpy dataChanged(&cache, &Cache::dataChanged);
        QSignalSpy destroyed(q, &InsertOrUpdateItemsQuery::destroyed);

        q->add(&lib);
        q->add(todoList);
        q->add(todo);
        q->add(task);
        q->add(note);
        q->add(image);
        cache.run(q);

        QVERIFY(cacheFinished.wait());
        QCOMPARE(destroyed.count(), 1);
        QCOMPARE(finished.count(), 1);
        QCOMPARE(dataChanged.count(), 0);
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
        QSignalSpy cacheFinished(&cache, &Cache::finished);
        QSignalSpy dataChanged(&cache, &Cache::dataChanged);
        QSignalSpy destroyed(q, &InsertOrUpdateItemsQuery::destroyed);

        q->add(&lib);
        q->add(todoList);
        q->add(todo);
        q->add(task);
        q->add(note);
        q->add(image);
        cache.run(q);

        QVERIFY(cacheFinished.wait());
        QCOMPARE(destroyed.count(), 1);
        QCOMPARE(finished.count(), 1);
        QCOMPARE(dataChanged.count(), 1);
    }
}

QTEST_MAIN(InsertOrUpdateItemsQueryTest)
#include "test_insertorupdateitemsquery.moc"

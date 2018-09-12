#include <QObject>
#include <QRegularExpression>
#include <QSet>
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
#include "datastorage/deleteitemsquery.h"
#include "datastorage/getitemsquery.h"
#include "datastorage/insertorupdateitemsquery.h"

class DeleteItemsQueryTest : public QObject
{
  Q_OBJECT

private slots:

  void initTestCase() {}
  void init() {}
  void run();
  void cleanup() {}
  void cleanupTestCase() {}
};




void DeleteItemsQueryTest::run()
{
    QTest::ignoreMessage(QtDebugMsg,
                         QRegularExpression(".*Cache is uninitialized.*"));
    QTemporaryDir tmpDir;
    Cache cache;
    cache.setCacheDirectory(tmpDir.path());
    cache.setCacheSize(1024*1024);
    QVERIFY(cache.open());

    {
        auto q = new DeleteItemsQuery();
        QSignalSpy itemDeleted(
                    q, &DeleteItemsQuery::itemDeleted);
        QSignalSpy destroyed(q, &GetItemsQuery::destroyed);
        cache.run(q);
        QVERIFY(destroyed.wait());
        QCOMPARE(itemDeleted.count(), 0);
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
        auto q = new GetItemsQuery();
        QSignalSpy itemsAvailable(
                    q, &GetItemsQuery::itemsAvailable);
        QSignalSpy destroyed(q, &GetItemsQuery::destroyed);
        cache.run(q);
        QVERIFY(destroyed.wait());
        QCOMPARE(itemsAvailable.count(), 1);
        auto items = itemsAvailable.at(0).at(0).toList();
        QCOMPARE(items.count(), 5);
        QSet<QByteArray> got = QSet<QByteArray>(
        {
                        items.at(0).value<ItemCacheEntry>().toByteArray(),
                        items.at(1).value<ItemCacheEntry>().toByteArray(),
                        items.at(2).value<ItemCacheEntry>().toByteArray(),
                        items.at(3).value<ItemCacheEntry>().toByteArray(),
                        items.at(4).value<ItemCacheEntry>().toByteArray(),
                    });
        QSet<QByteArray> expected = QSet<QByteArray>(
        {
                        todoList->encache().toByteArray(),
                        todo->encache().toByteArray(),
                        task->encache().toByteArray(),
                        note->encache().toByteArray(),
                        image->encache().toByteArray(),
                    });
        QCOMPARE(got, expected);
    }

    {
        auto q = new DeleteItemsQuery();
        q->deleteItem(task);
        QSignalSpy itemDeleted(q, &DeleteItemsQuery::itemDeleted);
        QSignalSpy destroyed(q, &DeleteItemsQuery::destroyed);
        cache.run(q);
        QVERIFY(destroyed.wait());
        QCOMPARE(destroyed.count(), 1);
    }

    {
        auto q = new GetItemsQuery();
        QSignalSpy itemsAvailable(
                    q, &GetItemsQuery::itemsAvailable);
        QSignalSpy destroyed(q, &GetItemsQuery::destroyed);
        cache.run(q);
        QVERIFY(destroyed.wait());
        QCOMPARE(itemsAvailable.count(), 1);
        auto items = itemsAvailable.at(0).at(0).toList();
        QCOMPARE(items.count(), 4);
        QSet<QByteArray> got = QSet<QByteArray>(
        {
                        items.at(0).value<ItemCacheEntry>().toByteArray(),
                        items.at(1).value<ItemCacheEntry>().toByteArray(),
                        items.at(2).value<ItemCacheEntry>().toByteArray(),
                        items.at(3).value<ItemCacheEntry>().toByteArray(),
                    });
        QSet<QByteArray> expected = QSet<QByteArray>(
        {
                        todoList->encache().toByteArray(),
                        todo->encache().toByteArray(),
                        note->encache().toByteArray(),
                        image->encache().toByteArray(),
                    });
        QCOMPARE(got, expected);
    }

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
        auto q = new DeleteItemsQuery();
        q->deleteItem(todo);
        QSignalSpy itemDeleted(q, &DeleteItemsQuery::itemDeleted);
        QSignalSpy destroyed(q, &DeleteItemsQuery::destroyed);
        cache.run(q);
        QVERIFY(destroyed.wait());
        QCOMPARE(destroyed.count(), 1);
    }

    {
        auto q = new GetItemsQuery();
        QSignalSpy itemsAvailable(
                    q, &GetItemsQuery::itemsAvailable);
        QSignalSpy destroyed(q, &GetItemsQuery::destroyed);
        cache.run(q);
        QVERIFY(destroyed.wait());
        QCOMPARE(itemsAvailable.count(), 1);
        auto items = itemsAvailable.at(0).at(0).toList();
        QCOMPARE(items.count(), 3);
        QSet<QByteArray> got = QSet<QByteArray>(
        {
                        items.at(0).value<ItemCacheEntry>().toByteArray(),
                        items.at(1).value<ItemCacheEntry>().toByteArray(),
                        items.at(2).value<ItemCacheEntry>().toByteArray(),
                    });
        QSet<QByteArray> expected = QSet<QByteArray>(
        {
                        todoList->encache().toByteArray(),
                        note->encache().toByteArray(),
                        image->encache().toByteArray(),
                    });
        QCOMPARE(got, expected);
    }

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
        auto q = new DeleteItemsQuery();
        q->deleteItem(todoList);
        QSignalSpy itemDeleted(q, &DeleteItemsQuery::itemDeleted);
        QSignalSpy destroyed(q, &DeleteItemsQuery::destroyed);
        cache.run(q);
        QVERIFY(destroyed.wait());
        QCOMPARE(destroyed.count(), 1);
    }

    {
        auto q = new GetItemsQuery();
        QSignalSpy itemsAvailable(
                    q, &GetItemsQuery::itemsAvailable);
        QSignalSpy destroyed(q, &GetItemsQuery::destroyed);
        cache.run(q);
        QVERIFY(destroyed.wait());
        QCOMPARE(itemsAvailable.count(), 1);
        auto items = itemsAvailable.at(0).at(0).toList();
        QCOMPARE(items.count(), 2);
        QSet<QByteArray> got = QSet<QByteArray>(
        {
                        items.at(0).value<ItemCacheEntry>().toByteArray(),
                        items.at(1).value<ItemCacheEntry>().toByteArray(),
                    });
        QSet<QByteArray> expected = QSet<QByteArray>(
        {
                        note->encache().toByteArray(),
                        image->encache().toByteArray(),
                    });
        QCOMPARE(got, expected);
    }

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
        auto q = new DeleteItemsQuery();
        q->deleteLibrary(&lib);
        QSignalSpy itemDeleted(q, &DeleteItemsQuery::itemDeleted);
        QSignalSpy destroyed(q, &DeleteItemsQuery::destroyed);
        cache.run(q);
        QVERIFY(destroyed.wait());
        QCOMPARE(destroyed.count(), 1);
    }

    {
        auto q = new GetItemsQuery();
        QSignalSpy itemsAvailable(
                    q, &GetItemsQuery::itemsAvailable);
        QSignalSpy destroyed(q, &GetItemsQuery::destroyed);
        cache.run(q);
        QVERIFY(destroyed.wait());
        QCOMPARE(itemsAvailable.count(), 1);
        auto items = itemsAvailable.at(0).at(0).toList();
        QCOMPARE(items.count(), 0);
    }
}

QTEST_MAIN(DeleteItemsQueryTest)
#include "test_deleteitemsquery.moc"

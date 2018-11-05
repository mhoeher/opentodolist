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
//    QTest::ignoreMessage(QtDebugMsg,
//                         QRegularExpression(".*Cache is uninitialized.*"));
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
    TodoList todoList;
    todoList.setTitle("A todo list");
    todoList.setLibraryId(lib.uid());
    Todo todo;
    todo.setTitle("A todo");
    todo.setTodoListUid(todoList.uid());
    Task task;
    task.setTitle("A task");
    task.setTodoUid(todo.uid());
    Note note;
    note.setTitle("A note");
    note.setLibraryId(lib.uid());
    Image image;
    image.setTitle("An image");
    image.setLibraryId(lib.uid());

    {
        auto q = new InsertOrUpdateItemsQuery;
        QSignalSpy finished(q, &InsertOrUpdateItemsQuery::finished);
        QSignalSpy cacheFinished(&cache, &Cache::finished);

        q->add(&lib);
        q->add(&todoList);
        q->add(&todo);
        q->add(&task);
        q->add(&note);
        q->add(&image);
        cache.run(q);

        QVERIFY(cacheFinished.wait());
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
                        todoList.encache().toByteArray(),
                        todo.encache().toByteArray(),
                        task.encache().toByteArray(),
                        note.encache().toByteArray(),
                        image.encache().toByteArray(),
                    });
        QCOMPARE(got, expected);
    }

    {
        auto q = new DeleteItemsQuery();
        q->deleteItem(&task);
        QSignalSpy itemDeleted(q, &DeleteItemsQuery::itemDeleted);
        QSignalSpy cacheFinished(&cache, &Cache::finished);
        QSignalSpy dataChanged(&cache, &Cache::dataChanged);
        QSignalSpy librariesChanged(&cache, &Cache::librariesChanged);
        cache.run(q);
        QVERIFY(cacheFinished.wait());
        QCOMPARE(dataChanged.count(), 1);
        QCOMPARE(librariesChanged.count(), 1);
        QCOMPARE(librariesChanged.at(0).at(0).toList().at(0).toUuid(),
                 lib.uid());
    }

    {
        auto q = new DeleteItemsQuery();
        q->deleteItem(&task);
        QSignalSpy itemDeleted(q, &DeleteItemsQuery::itemDeleted);
        QSignalSpy cacheFinished(&cache, &Cache::finished);
        QSignalSpy dataChanged(&cache, &Cache::dataChanged);
        QSignalSpy librariesChanged(&cache, &Cache::librariesChanged);
        cache.run(q);
        QVERIFY(cacheFinished.wait());
        QCOMPARE(dataChanged.count(), 0);
        QCOMPARE(librariesChanged.count(), 0);
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
                        todoList.encache().toByteArray(),
                        todo.encache().toByteArray(),
                        note.encache().toByteArray(),
                        image.encache().toByteArray(),
                    });
        QCOMPARE(got, expected);
    }

    {
        auto q = new InsertOrUpdateItemsQuery;
        QSignalSpy finished(q, &InsertOrUpdateItemsQuery::finished);
        QSignalSpy destroyed(q, &InsertOrUpdateItemsQuery::destroyed);

        q->add(&lib);
        q->add(&todoList);
        q->add(&todo);
        q->add(&task);
        q->add(&note);
        q->add(&image);
        cache.run(q);

        QVERIFY(destroyed.wait());
        QCOMPARE(finished.count(), 1);
    }

    {
        auto q = new DeleteItemsQuery();
        q->deleteItem(&todo);
        QSignalSpy cacheFinished(&cache, &Cache::finished);
        QSignalSpy dataChanged(&cache, &Cache::dataChanged);
        QSignalSpy itemDeleted(q, &DeleteItemsQuery::itemDeleted);
        QSignalSpy destroyed(q, &DeleteItemsQuery::destroyed);
        cache.run(q);
        QVERIFY(cacheFinished.wait());
        QCOMPARE(dataChanged.count(), 1);
        if (destroyed.count() == 0) {
            QVERIFY(destroyed.wait());
        }
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
                        todoList.encache().toByteArray(),
                        note.encache().toByteArray(),
                        image.encache().toByteArray(),
                    });
        QCOMPARE(got, expected);
    }

    {
        auto q = new InsertOrUpdateItemsQuery;
        QSignalSpy finished(q, &InsertOrUpdateItemsQuery::finished);
        QSignalSpy destroyed(q, &InsertOrUpdateItemsQuery::destroyed);

        q->add(&lib);
        q->add(&todoList);
        q->add(&todo);
        q->add(&task);
        q->add(&note);
        q->add(&image);
        cache.run(q);

        QVERIFY(destroyed.wait());
        QCOMPARE(finished.count(), 1);
    }

    {
        auto q = new DeleteItemsQuery();
        q->deleteItem(&todoList);
        QSignalSpy itemDeleted(q, &DeleteItemsQuery::itemDeleted);
        QSignalSpy destroyed(q, &DeleteItemsQuery::destroyed);
        QSignalSpy cacheFinished(&cache, &Cache::finished);
        QSignalSpy dataChanged(&cache, &Cache::dataChanged);
        cache.run(q);
        QVERIFY(cacheFinished.wait());
        QCOMPARE(dataChanged.count(), 1);
        if (destroyed.count() == 0) {
            QVERIFY(destroyed.wait());
        }
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
                        note.encache().toByteArray(),
                        image.encache().toByteArray(),
                    });
        QCOMPARE(got, expected);
    }

    {
        auto q = new InsertOrUpdateItemsQuery;
        QSignalSpy finished(q, &InsertOrUpdateItemsQuery::finished);
        QSignalSpy destroyed(q, &InsertOrUpdateItemsQuery::destroyed);

        q->add(&lib);
        q->add(&todoList);
        q->add(&todo);
        q->add(&task);
        q->add(&note);
        q->add(&image);
        cache.run(q);

        QVERIFY(destroyed.wait());
        QCOMPARE(finished.count(), 1);
    }

    {
        auto q = new DeleteItemsQuery();
        q->deleteLibrary(&lib, false);
        QSignalSpy itemDeleted(q, &DeleteItemsQuery::itemDeleted);
        QSignalSpy destroyed(q, &DeleteItemsQuery::destroyed);
        QSignalSpy cacheFinished(&cache, &Cache::finished);
        QSignalSpy dataChanged(&cache, &Cache::dataChanged);
        cache.run(q);
        QVERIFY(cacheFinished.wait());
        QCOMPARE(dataChanged.count(), 1);
        if (destroyed.count() == 0) {
            QVERIFY(destroyed.wait());
        }
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

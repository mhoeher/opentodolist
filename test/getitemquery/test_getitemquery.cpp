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
#include "datastorage/getitemquery.h"
#include "datastorage/insertorupdateitemsquery.h"

class GetItemQueryTest : public QObject
{
  Q_OBJECT

private slots:

  void initTestCase() {}
  void init() {}
  void run();
  void cleanup() {}
  void cleanupTestCase() {}
};




void GetItemQueryTest::run()
{
    QTemporaryDir tmpDir;
    Cache cache;
    cache.setCacheDirectory(tmpDir.path());
    cache.setCacheSize(1024*1024);
    QVERIFY(cache.open());

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
        auto q = new GetItemQuery();
        q->setUid(todoList.uid());
        QSignalSpy itemLoaded(q, &GetItemQuery::itemLoaded);
        cache.run(q);
        QVERIFY(itemLoaded.wait());
        ItemPtr item(Item::decache(itemLoaded.value(0).value(0)));
        QVERIFY(item != nullptr);
        QCOMPARE(item->uid(), todoList.uid());
        QCOMPARE(item->title(), todoList.title());
    }

    {
        auto q = new GetItemQuery();
        q->setUid(todo.uid());
        QSignalSpy itemLoaded(q, &GetItemQuery::itemLoaded);
        cache.run(q);
        QVERIFY(itemLoaded.wait());
        ItemPtr item(Item::decache(itemLoaded.value(0).value(0)));
        QVERIFY(item != nullptr);
        QCOMPARE(item->uid(), todo.uid());
        QCOMPARE(item->title(), todo.title());
    }

    {
        auto q = new GetItemQuery();
        q->setUid(task.uid());
        QSignalSpy itemLoaded(q, &GetItemQuery::itemLoaded);
        cache.run(q);
        QVERIFY(itemLoaded.wait());
        ItemPtr item(Item::decache(itemLoaded.value(0).value(0)));
        QVERIFY(item != nullptr);
        QCOMPARE(item->uid(), task.uid());
        QCOMPARE(item->title(), task.title());
    }

    {
        auto q = new GetItemQuery();
        q->setUid(note.uid());
        QSignalSpy itemLoaded(q, &GetItemQuery::itemLoaded);
        cache.run(q);
        QVERIFY(itemLoaded.wait());
        ItemPtr item(Item::decache(itemLoaded.value(0).value(0)));
        QVERIFY(item != nullptr);
        QCOMPARE(item->uid(), note.uid());
        QCOMPARE(item->title(), note.title());
    }

    {
        auto q = new GetItemQuery();
        q->setUid(image.uid());
        QSignalSpy itemLoaded(q, &GetItemQuery::itemLoaded);
        cache.run(q);
        QVERIFY(itemLoaded.wait());
        ItemPtr item(Item::decache(itemLoaded.value(0).value(0)));
        QVERIFY(item != nullptr);
        QCOMPARE(item->uid(), image.uid());
        QCOMPARE(item->title(), image.title());
    }
}

QTEST_MAIN(GetItemQueryTest)
#include "test_getitemquery.moc"

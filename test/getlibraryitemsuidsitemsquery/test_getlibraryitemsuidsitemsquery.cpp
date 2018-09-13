
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
#include "datastorage/getlibraryitemsuidsitemsquery.h"
#include "datastorage/insertorupdateitemsquery.h"

class GetLibraryItemsUIDsItemsQueryTest : public QObject
{
  Q_OBJECT

private slots:

  void initTestCase() {}
  void init() {}
  void run();
  void cleanup() {}
  void cleanupTestCase() {}
};




void GetLibraryItemsUIDsItemsQueryTest::run()
{
//    QTest::ignoreMessage(QtDebugMsg,
//                         QRegularExpression(".*Cache is uninitialized.*"));
    QTemporaryDir tmpDir;
    Cache cache;
    cache.setCacheDirectory(tmpDir.path());
    cache.setCacheSize(1024*1024);
    QVERIFY(cache.open());

    {
        auto q = new GetLibraryItemsUIDsItemsQuery();
        QSignalSpy uidsAvailable(
                    q, &GetLibraryItemsUIDsItemsQuery::uidsAvailable);
        QSignalSpy destroyed(q, &GetLibraryItemsUIDsItemsQuery::destroyed);
        cache.run(q);
        QVERIFY(destroyed.wait());
        QCOMPARE(uidsAvailable.count(), 1);
        QCOMPARE(uidsAvailable.at(0).at(0).toList().count(), 0);
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
        auto q = new GetLibraryItemsUIDsItemsQuery();
        q->addLibrary(&lib);
        QSignalSpy uidsAvailable(
                    q, &GetLibraryItemsUIDsItemsQuery::uidsAvailable);
        QSignalSpy destroyed(q, &GetLibraryItemsUIDsItemsQuery::destroyed);
        cache.run(q);
        QVERIFY(destroyed.wait());
        QCOMPARE(uidsAvailable.count(), 1);
        auto got = uidsAvailable.at(0).at(0).value<QSet<QUuid>>();
        auto expected = QSet<QUuid>(
        {
                        todoList->uid(),
                        todo->uid(),
                        task->uid(),
                        note->uid(),
                        image->uid(),
                    });
        QCOMPARE(got, expected);
    }
}

QTEST_MAIN(GetLibraryItemsUIDsItemsQueryTest)
#include "test_getlibraryitemsuidsitemsquery.moc"

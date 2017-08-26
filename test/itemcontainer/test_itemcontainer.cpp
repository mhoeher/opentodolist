#include "itemcontainer.h"

#include <QObject>
#include <QSignalSpy>
#include <QTest>

#include "todolist.h"
#include "todo.h"
#include "task.h"
#include "note.h"
#include "image.h"

class ItemContainerTest : public QObject
{
  Q_OBJECT

private slots:

  void initTestCase() {}
  void init();
  void testAddItem();
  void testCount();
  void testUpdateItem();
  void testDeleteItem();
  void testItemChanged();
  void cleanup();
  void cleanupTestCase() {}

private:

  TodoListPtr       m_todoList;
  TodoPtr           m_todo;
  TaskPtr           m_task;
  NotePtr           m_note;
  ImagePtr          m_image;
  QList<ItemPtr>    m_items;

};




void ItemContainerTest::init()
{
    m_todoList = TodoListPtr::create();
    m_todo = TodoPtr::create();
    m_task = TaskPtr::create();
    m_note = NotePtr::create();
    m_image = ImagePtr::create();
    m_items = QList<ItemPtr>({
                                 m_todoList,
                                 m_todo,
                                 m_task,
                                 m_note,
                                 m_image
                             });
}

void ItemContainerTest::testAddItem()
{
    ItemContainer c;
    int expectedIndex = 0;
    for (auto item : m_items) {
        QSignalSpy itemAdded(&c, &ItemContainer::itemAdded);
        c.addItem(item);
        QVERIFY(itemAdded.wait(1000));
        QCOMPARE(itemAdded.at(0).at(0).toInt(), expectedIndex++);
    }
}

void ItemContainerTest::testCount()
{
    ItemContainer c;
    int expectedCount = 0;
    for (auto item : m_items) {
        QSignalSpy itemAdded(&c, &ItemContainer::itemAdded);
        QCOMPARE(c.count(), expectedCount);
        c.addItem(item);
        QVERIFY(itemAdded.wait(1000));
        QCOMPARE(c.count(), ++expectedCount);
    }
}

void ItemContainerTest::testUpdateItem()
{
    ItemContainer c;
    for (auto item : m_items) {
        QSignalSpy itemAdded(&c, &ItemContainer::itemAdded);
        c.addItem(item);
        QVERIFY(itemAdded.wait(1000));
    }
    TaskPtr task(new Task());
    task->fromVariant(m_task->toVariant());
    QSignalSpy titleChanged(m_task.data(), &Task::titleChanged);
    task->setTitle("Test Task");
    c.updateItem(task);
    QVERIFY(titleChanged.wait(1000));

    c.updateItem(TaskPtr(new Task())); // No effect
}

void ItemContainerTest::testDeleteItem()
{
    ItemContainer c;
    for (auto item : m_items) {
        QSignalSpy itemAdded(&c, &ItemContainer::itemAdded);
        c.addItem(item);
        QVERIFY(itemAdded.wait(1000));
    }
    const QList<ItemPtr> ItemsToRemove = {
      m_task, m_note, m_todoList, m_image, m_todo
    };
    const int ExpectedIndices[] = {
        2, 2, 0, 1, 0
    };
    for (int i = 0; i < ItemsToRemove.length(); ++i) {
        QSignalSpy itemDeleted(&c, &ItemContainer::itemDeleted);
        c.deleteItem(ItemsToRemove.at(i));
        QVERIFY(itemDeleted.wait(1000));
        QCOMPARE(itemDeleted.at(0).at(0).toInt(), ExpectedIndices[i]);
    }
}

void ItemContainerTest::testItemChanged()
{
    ItemContainer c;
    QSignalSpy itemAdded(&c, &ItemContainer::itemAdded);
    auto item1 = m_items.at(0);
    auto item2 = m_items.at(1);
    c.addItem(item1);
    QVERIFY(itemAdded.wait(1000));
    c.addItem(item2);
    QVERIFY(itemAdded.wait(1000));
    QSignalSpy spy(&c, &ItemContainer::itemChanged);
    item1->setTitle("Foo");
    QVERIFY(spy.wait(1000));
    item2->setTitle("Bar");
    QVERIFY(spy.wait(1000));
    QCOMPARE(spy.count(), 2);
    auto row = spy.at(0);
    QCOMPARE(row.count(), 1);
    QCOMPARE(row.at(0).toInt(), 0);
    row = spy.at(1);
    QCOMPARE(row.count(), 1);
    QCOMPARE(row.at(0).toInt(), 1);
}

void ItemContainerTest::cleanup()
{
    m_todoList.clear();
    m_todo.clear();
    m_task.clear();
    m_note.clear();
    m_image.clear();
    m_items.clear();
}

QTEST_MAIN(ItemContainerTest)
#include "test_itemcontainer.moc"

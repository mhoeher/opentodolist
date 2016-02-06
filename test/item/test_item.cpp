#include "item.h"

#include <QDir>
#include <QObject>
#include <QTest>
#include <QTemporaryDir>

class ItemTest : public QObject
{
  Q_OBJECT
  
private slots:
  
  void initTestCase() {}
  void testConstructor();
  void testPersistence();
  void testDeleteItem();
  void cleanupTestCase() {}
  
};



void ItemTest::testConstructor()
{
  QTemporaryDir dir;
  QVERIFY2(dir.isValid(), "Failed to create temporary directory.");
  Item item(dir.path());
  QCOMPARE(item.directory(), dir.path());
}

void ItemTest::testPersistence()
{
  QTemporaryDir dir;
  QVERIFY2(dir.isValid(), "Failed to create temporary directory.");
  auto item = new Item(dir.path());
  Q_CHECK_PTR(item);
  item->setTitle("TestItem");
  QUuid uid = item->uid();
  item->commitItem();
  delete item;
  item = new Item(dir.path());
  Q_CHECK_PTR(item);
  QCOMPARE(item->title(), QString("TestItem"));
  QCOMPARE(item->uid(), uid);
  delete item;
}

void ItemTest::testDeleteItem()
{
  QTemporaryDir dir;
  QVERIFY2(dir.isValid(), "Failed to create temporary directory.");
  QString itemDir = dir.path() + "/itemdir";
  auto item = new Item(itemDir);
  Q_CHECK_PTR(item);
  QVERIFY2(QDir(itemDir).exists(), "Item was not created in the expected location.");
  bool itemDeletedSignalEmitted = false;
  connect(item, &Item::itemDeleted, 
          [&itemDeletedSignalEmitted](Item*) { itemDeletedSignalEmitted = true; });
  QVERIFY2(item->deleteItem(), "Failed to delete item.");
  QVERIFY2(itemDeletedSignalEmitted, "The Item::itemDeleted() signal was not emitted.");
  QVERIFY2(!QDir(itemDir).exists(), "Item directory should have been deleted!");
  delete item;
}

QTEST_MAIN(ItemTest)
#include "test_item.moc"

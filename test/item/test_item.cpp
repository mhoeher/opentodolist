#include "item.h"

#include <QObject>
#include <QTest>
#include <QTemporaryDir>

class ItemTest : public QObject
{
  Q_OBJECT
  
private slots:
  
  void initTestCase() {}
  void testConstructor();
  void testPersisistence();
  void cleanupTestCase() {}
  
};



void ItemTest::testConstructor()
{
  QTemporaryDir dir;
  QVERIFY2(dir.isValid(), "Failed to create temporary directory.");
  Item item(dir.path());
  QCOMPARE(item.directory(), dir.path());
}

void ItemTest::testPersisistence()
{
  QTemporaryDir dir;
  QVERIFY2(dir.isValid(), "Failed to create temporary directory.");
  auto item = new Item(dir.path());
  Q_CHECK_PTR(item);
  item->setTitle("TestItem");
  QUuid uid = item->uid();
  delete item;
  item = new Item(dir.path());
  Q_CHECK_PTR(item);
  QCOMPARE(item->title(), QString("TestItem"));
  QCOMPARE(item->uid(), uid);
  delete item;
}

QTEST_MAIN(ItemTest)
#include "test_item.moc"

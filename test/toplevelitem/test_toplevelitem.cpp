#include "toplevelitem.h"

#include <QTemporaryDir>
#include <QObject>
#include <QTest>

class TopLevelItemTest : public QObject
{
  Q_OBJECT
  
private slots:
  
  void initTestCase() {}
  void testConstructor();
  void testPersistence();
  void cleanupTestCase() {}
};

void TopLevelItemTest::testConstructor()
{
  QTemporaryDir dir;
  QVERIFY2(dir.isValid(), "Unable to create temporary directory.");
  auto item = new TopLevelItem(dir.path());
  Q_CHECK_PTR(item);
  QCOMPARE(item->itemType(), TopLevelItem::ItemType);
  delete item;
}

void TopLevelItemTest::testPersistence()
{
  QTemporaryDir dir;
  QVERIFY2(dir.isValid(), "Unable to create temporary directory.");
  auto item = new TopLevelItem(dir.path());
  Q_CHECK_PTR(item);
  TopLevelItem::Color color = TopLevelItem::Green;
  item->setColor(color);
  delete item;
  item = new TopLevelItem(dir.path());
  Q_CHECK_PTR(item);
  QCOMPARE(item->color(), color);
  delete item;
}

QTEST_MAIN(TopLevelItemTest)
#include "test_toplevelitem.moc"

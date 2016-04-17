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
  void testTags();
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
  item->setTags({"Tag1", "Tag2"});
  item->commitItem();
  delete item;
  item = new TopLevelItem(dir.path());
  Q_CHECK_PTR(item);
  QCOMPARE(item->color(), color);
  QCOMPARE(item->tags(), QStringList({"Tag1", "Tag2"}));
  delete item;
}

void TopLevelItemTest::testTags()
{
    TopLevelItem item;
    QCOMPARE(item.tags(), QStringList());
    item.addTag("Tag 2");
    QCOMPARE(item.tags(), QStringList({"Tag 2"}));
    item.addTag("Tag 1");
    QCOMPARE(item.tags(), QStringList({"Tag 1", "Tag 2"}));
    item.removeTagAt(1);
    QCOMPARE(item.tags(), QStringList({"Tag 1"}));
}

QTEST_MAIN(TopLevelItemTest)
#include "test_toplevelitem.moc"

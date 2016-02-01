#include "complexitem.h"

#include <QTemporaryDir>
#include <QObject>
#include <QTest>

class ComplexItemTest : public QObject
{
  Q_OBJECT
  
private slots:
  
  void initTestCase() {}
  void testConstructor();
  void testPersistence();
  void cleanupTestCase() {}
};

void ComplexItemTest::testConstructor()
{
  QTemporaryDir dir;
  QVERIFY2(dir.isValid(), "Unable to create temporary directory.");
  auto item = new ComplexItem(dir.path());
  Q_CHECK_PTR(item);
  QCOMPARE(item->itemType(), ComplexItem::ItemType);
  delete item;
}

void ComplexItemTest::testPersistence()
{
  QTemporaryDir dir;
  QVERIFY2(dir.isValid(), "Unable to create temporary directory.");
  auto item = new ComplexItem(dir.path());
  Q_CHECK_PTR(item);
  QDateTime dueTo = QDateTime::currentDateTime();
  QString notes = "Test Notes";
  item->setDueTo(dueTo);
  item->setNotes(notes);
  // Date times are stored only at a second basis, so read back:
  dueTo = item->dueTo();
  delete item;
  item = new ComplexItem(dir.path());
  Q_CHECK_PTR(item);
  QCOMPARE(item->dueTo(), dueTo);
  QCOMPARE(item->notes(), notes);
  delete item;
}

QTEST_MAIN(ComplexItemTest)
#include "test_complexitem.moc"

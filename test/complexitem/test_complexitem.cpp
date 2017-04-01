#include "complexitem.h"

#include <QTemporaryDir>
#include <QObject>
#include <QSignalSpy>
#include <QTest>

class ComplexItemTest : public QObject
{
  Q_OBJECT

private slots:

  void initTestCase() {}
  void testProperties();
  void testPersistence();
  void cleanupTestCase() {}
};

void ComplexItemTest::testProperties()
{
    ComplexItem item;
    QSignalSpy dueToChanged(&item, &ComplexItem::dueToChanged);
    QSignalSpy notesChanged(&item, &ComplexItem::notesChanged);

    item.setDueTo(QDateTime::currentDateTime());
    item.setNotes("Hello World");

    QCOMPARE(dueToChanged.count(), 1);
    QCOMPARE(notesChanged.count(), 1);
}

void ComplexItemTest::testPersistence()
{
    ComplexItem item, anotherItem;
    QSignalSpy dueToChanged(&anotherItem, &ComplexItem::dueToChanged);
    QSignalSpy notesChanged(&anotherItem, &ComplexItem::notesChanged);

    item.setDueTo(QDateTime::currentDateTime());
    item.setNotes("Hello World");

    anotherItem.fromVariant(item.toVariant());

    QCOMPARE(dueToChanged.count(), 1);
    QCOMPARE(notesChanged.count(), 1);

    QCOMPARE(item.dueTo(), anotherItem.dueTo());
    QCOMPARE(item.notes(), anotherItem.notes());
}

QTEST_MAIN(ComplexItemTest)
#include "test_complexitem.moc"

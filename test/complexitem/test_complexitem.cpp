#include "datamodel/complexitem.h"

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
  void attachments();
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

void ComplexItemTest::attachments()
{
    QTemporaryDir dir;
    QDir d(dir.path());
    d.mkdir("item");
    d.mkdir("res");
    ComplexItem item(d.absoluteFilePath("item/item.otl"));
    QSignalSpy attachmentsChanged(&item, &ComplexItem::attachmentsChanged);
    QFile file(d.absoluteFilePath("res/foo.txt"));
    QVERIFY(file.open(QIODevice::WriteOnly));
    file.write("Hello World");
    file.close();

    item.attachFile(d.absoluteFilePath("res/foo.txt"));
    QCOMPARE(attachmentsChanged.count(), 1);
    QCOMPARE(item.attachments(), QStringList({"foo.txt"}));
    QVERIFY(QFile(item.attachmentFileName("foo.txt")).exists());

    item.attachFile(d.absoluteFilePath("res/foo.txt"));
    QCOMPARE(attachmentsChanged.count(), 2);
    QCOMPARE(item.attachments(), QStringList({"foo-1.txt", "foo.txt"}));
    QVERIFY(QFile(item.attachmentFileName("foo-1.txt")).exists());

    item.detachFile("foo.txt");
    QCOMPARE(attachmentsChanged.count(), 3);
    QCOMPARE(item.attachments(), QStringList({"foo-1.txt"}));
    QVERIFY(QFile(item.attachmentFileName("foo-1.txt")).exists());
    QVERIFY(!QFile(item.attachmentFileName("foo.txt")).exists());

    auto attachmentPath = item.attachmentFileName("foo-1.txt");
    QVERIFY(QFile(attachmentPath).exists());
    QVERIFY(item.deleteItem());
    QVERIFY(!QFile(attachmentPath).exists());

}

QTEST_MAIN(ComplexItemTest)
#include "test_complexitem.moc"

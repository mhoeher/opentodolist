#include "datamodel/item.h"

#include <QDir>
#include <QObject>
#include <QSignalSpy>
#include <QTest>
#include <QTemporaryDir>

class ItemTest : public QObject
{
    Q_OBJECT

private slots:

    void initTestCase() {}
    void init();
    void testProperties();
    void testPersistence();
    void testDeleteItem();
    void cleanup();
    void cleanupTestCase() {}

private:

    QTemporaryDir *m_dir;

};


void ItemTest::init()
{
    m_dir = new QTemporaryDir();
}

void ItemTest::testProperties()
{
    Item item;
    QSignalSpy uidChanged(&item, &Item::uidChanged);
    QSignalSpy titleChanged(&item, &Item::titleChanged);
    QSignalSpy filenameChanged(&item, &Item::filenameChanged);
    QSignalSpy weightChanged(&item, &Item::weightChanged);

    item.setTitle("Hello World");
    item.setWeight(1.0);

    QCOMPARE(item.title(), QString("Hello World"));
    QVERIFY(qFuzzyCompare(item.weight(), 1.0));

    QCOMPARE(uidChanged.count(), 0);
    QCOMPARE(titleChanged.count(), 1);
    QCOMPARE(filenameChanged.count(), 0);
    QCOMPARE(weightChanged.count(), 1);
}

void ItemTest::testPersistence()
{
    Item item(QDir(m_dir->path())), anotherItem;
    QSignalSpy uidChanged(&anotherItem, &Item::uidChanged);
    QSignalSpy titleChanged(&anotherItem, &Item::titleChanged);
    QSignalSpy filenameChanged(&anotherItem, &Item::filenameChanged);
    QSignalSpy weightChanged(&anotherItem, &Item::weightChanged);

    item.setTitle("Hello World");
    item.setWeight(1.0);

    anotherItem.fromVariant(item.toVariant());

    QCOMPARE(anotherItem.uid(), item.uid());
    QCOMPARE(anotherItem.title(), item.title());
    QCOMPARE(anotherItem.filename(), item.filename());
    QCOMPARE(anotherItem.weight(), item.weight());

    QCOMPARE(uidChanged.count(), 1);
    QCOMPARE(titleChanged.count(), 1);
    QCOMPARE(filenameChanged.count(), 1);
    QCOMPARE(weightChanged.count(), 1);

    Item thirdItem(item.filename());
    thirdItem.load();
    QCOMPARE(thirdItem.uid(), item.uid());
    QCOMPARE(thirdItem.title(), item.title());
    QCOMPARE(thirdItem.filename(), item.filename());
    QCOMPARE(thirdItem.weight(), item.weight());
}

void ItemTest::testDeleteItem()
{
    Item item(QDir(m_dir->path()));
    QVERIFY(item.save());
    {
        QDir dir(m_dir->path());
        QCOMPARE(dir.entryList(QDir::Files).length(), 1);
    }
    QVERIFY(item.deleteItem());
    {
        QDir dir(m_dir->path());
        QCOMPARE(dir.entryList(QDir::Files).length(), 0);
    }
}

void ItemTest::cleanup()
{
    delete m_dir;
}

QTEST_MAIN(ItemTest)
#include "test_item.moc"

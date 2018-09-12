#include "datamodel/toplevelitem.h"

#include <QObject>
#include <QSignalSpy>
#include <QTemporaryDir>
#include <QTest>

class TopLevelItemTest : public QObject
{
    Q_OBJECT

private slots:

    void initTestCase() {}
    void testProperties();
    void testPersistence();
    void testTags();
    void cleanupTestCase() {}

};

void TopLevelItemTest::testProperties()
{
    TopLevelItem item;
    QSignalSpy colorChanged(&item, &TopLevelItem::colorChanged);
    QSignalSpy tagsChanged(&item, &TopLevelItem::tagsChanged);

    QCOMPARE(item.color(), TopLevelItem::White);

    item.setColor(TopLevelItem::Green);
    item.setTags({"Hello", "World"});

    QCOMPARE(colorChanged.count(), 1);
    QCOMPARE(tagsChanged.count(), 1);

    QCOMPARE(item.color(), TopLevelItem::Green);
    QCOMPARE(item.tags(), QStringList({"Hello", "World"}));
}

void TopLevelItemTest::testPersistence()
{
    TopLevelItem item, anotherItem;
    QSignalSpy colorChanged(&anotherItem, &TopLevelItem::colorChanged);
    QSignalSpy tagsChanged(&anotherItem, &TopLevelItem::tagsChanged);

    QCOMPARE(item.color(), TopLevelItem::White);

    item.setColor(TopLevelItem::Green);
    item.setTags({"Hello", "World"});

    anotherItem.fromVariant(item.toVariant());

    QCOMPARE(colorChanged.count(), 1);
    QCOMPARE(tagsChanged.count(), 1);

    QCOMPARE(anotherItem.color(), TopLevelItem::Green);
    QCOMPARE(anotherItem.tags(), QStringList({"Hello", "World"}));
}

void TopLevelItemTest::testTags()
{
    TopLevelItem item;
    QSignalSpy tagsChanged(&item, &TopLevelItem::tagsChanged);
    QCOMPARE(item.tags(), QStringList());
    item.addTag("Tag 2");
    QCOMPARE(item.tags(), QStringList({"Tag 2"}));
    item.addTag("Tag 1");
    QCOMPARE(item.tags(), QStringList({"Tag 1", "Tag 2"}));
    item.removeTagAt(1);
    QCOMPARE(item.tags(), QStringList({"Tag 1"}));
    QCOMPARE(tagsChanged.count(), 3);
}

QTEST_MAIN(TopLevelItemTest)
#include "test_toplevelitem.moc"

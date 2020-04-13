/*
 * Copyright 2020 Martin Hoeher <martin@rpdev.net>
 +
 * This file is part of OpenTodoList.
 *
 * OpenTodoList is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * OpenTodoList is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenTodoList.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QDir>
#include <QObject>
#include <QSignalSpy>
#include <QTest>
#include <QTemporaryDir>

#include "datamodel/item.h"
#include "datamodel/note.h"

class ItemTest : public QObject
{
    Q_OBJECT

private slots:

    void initTestCase() {}
    void init();
    void testProperties();
    void testPersistence();
    void testDeleteItem();
    void testEncache();
    void testDecache();
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
    item.save();

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

void ItemTest::testEncache()
{
    Note note(QDir(m_dir->path()));
    note.setTitle("Hello World");
    auto entry = note.encache();
    QVERIFY(entry.valid);
}

void ItemTest::testDecache()
{
    Note note(QDir(m_dir->path()));
    note.setTitle("Hello World");
    note.setLibraryId(QUuid::createUuid());
    auto entry = note.encache();

    QScopedPointer<Item> item(Item::decache(entry));
    QVERIFY(item != nullptr);
    QCOMPARE(item->title(), note.title());
    QCOMPARE(item->directory(), note.directory());
    QCOMPARE(item->filename(), note.filename());
    QCOMPARE(item->uid(), note.uid());
    QCOMPARE(item->parentId(), note.parentId());
}

void ItemTest::cleanup()
{
    delete m_dir;
}

QTEST_MAIN(ItemTest)
#include "test_item.moc"

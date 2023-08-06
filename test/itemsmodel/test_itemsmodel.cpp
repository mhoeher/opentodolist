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

#include <QObject>
#include <QSignalSpy>
#include <QTemporaryDir>
#include <QTest>

#include "datamodel/library.h"
#include "datamodel/note.h"
#include "datastorage/cache.h"
#include "models/itemsmodel.h"
#include "datastorage/insertorupdateitemsquery.h"
#include "datastorage/deleteitemsquery.h"

class ItemsModelTest : public QObject
{
    Q_OBJECT

private slots:

    void initTestCase() {}
    void init();
    void testAddItems();
    void testDeleteItems();
    void testFilterByTag();
    void testFilterByNoTags();
    void cleanup();
    void cleanupTestCase() {}

private:
    QTemporaryDir* m_tmpDir;
    Cache* m_cache;
};

void ItemsModelTest::init()
{
    m_tmpDir = new QTemporaryDir();
    m_cache = new Cache();
    m_cache->setCacheDirectory(m_tmpDir->path());
    QVERIFY(m_cache->open());
}

void ItemsModelTest::testAddItems()
{
    Library lib;
    Note note1, note2;
    note1.setLibraryId(lib.uid());
    note1.setTitle("Note 1");
    note2.setLibraryId(lib.uid());
    note2.setTitle("Note 2");

    Library anotherLib;
    Note anotherNote1, anotherNote2;
    anotherNote1.setLibraryId(anotherLib.uid());
    anotherNote1.setTitle("Another Note 1");
    anotherNote2.setLibraryId(anotherLib.uid());
    anotherNote2.setTitle("Another Note 2");

    {
        auto q = new InsertOrUpdateItemsQuery;
        QSignalSpy finished(q, &InsertOrUpdateItemsQuery::finished);
        q->add(&lib);
        q->add(&note1);
        q->add(&note2);
        q->add(&anotherLib);
        q->add(&anotherNote1);
        q->add(&anotherNote2);
        m_cache->run(q);
        QVERIFY(finished.wait());
    }

    ItemsModel model;
    QSignalSpy countChanged(&model, &ItemsModel::countChanged);
    model.setCache(m_cache);
    QVERIFY(countChanged.wait());
    QCOMPARE(model.count(), 4);

    model.setParentItem(lib.uid());
    QVERIFY(countChanged.wait());
    QCOMPARE(model.count(), 2);
    auto notePtr1 =
            qobject_cast<Note*>(model.data(model.index(0), ItemsModel::ItemRole).value<QObject*>());
    auto notePtr2 =
            qobject_cast<Note*>(model.data(model.index(1), ItemsModel::ItemRole).value<QObject*>());
    QVERIFY(notePtr1->title() == note1.title() || notePtr1->title() == note2.title());
    QVERIFY(notePtr2->title() == note1.title() || notePtr2->title() == note2.title());
    QVERIFY(notePtr1->title() != notePtr2->title());
}

void ItemsModelTest::testDeleteItems()
{
    Library lib;
    Note note1, note2;
    note1.setLibraryId(lib.uid());
    note1.setTitle("Note 1");
    note2.setLibraryId(lib.uid());
    note2.setTitle("Note 2");

    Library anotherLib;
    Note anotherNote1, anotherNote2;
    anotherNote1.setLibraryId(anotherLib.uid());
    anotherNote1.setTitle("Another Note 1");
    anotherNote2.setLibraryId(anotherLib.uid());
    anotherNote2.setTitle("Another Note 2");

    {
        auto q = new InsertOrUpdateItemsQuery;
        QSignalSpy finished(q, &InsertOrUpdateItemsQuery::finished);
        q->add(&lib);
        q->add(&note1);
        q->add(&note2);
        q->add(&anotherLib);
        q->add(&anotherNote1);
        q->add(&anotherNote2);
        m_cache->run(q);
        QVERIFY(finished.wait());
    }

    ItemsModel model;
    QSignalSpy countChanged(&model, &ItemsModel::countChanged);
    model.setCache(m_cache);
    model.setParentItem(lib.uid());
    QVERIFY(countChanged.wait());
    QCOMPARE(model.count(), 2);

    {
        auto q = new DeleteItemsQuery;
        QSignalSpy deleted(q, &DeleteItemsQuery::destroyed);
        q->deleteItem(&note1);
        m_cache->run(q);
        QVERIFY(deleted.wait());
    }

    QVERIFY(countChanged.wait());
    QCOMPARE(model.count(), 1);
    auto notePtr =
            qobject_cast<Note*>(model.data(model.index(0), ItemsModel::ItemRole).value<QObject*>());
    QCOMPARE(notePtr->title(), note2.title());

    {
        auto q = new DeleteItemsQuery;
        QSignalSpy deleted(q, &DeleteItemsQuery::destroyed);
        q->deleteLibrary(&lib, false);
        m_cache->run(q);
        QVERIFY(deleted.wait());
    }

    QVERIFY(countChanged.wait());
    QCOMPARE(model.count(), 0);
}

void ItemsModelTest::testFilterByTag()
{
    Library lib;
    Note note1, note2;
    note1.setLibraryId(lib.uid());
    note1.setTitle("Note 1");
    note2.setLibraryId(lib.uid());
    note2.setTitle("Note 2");
    note2.setTags({ "A Tag" });

    {
        auto q = new InsertOrUpdateItemsQuery;
        QSignalSpy finished(q, &InsertOrUpdateItemsQuery::finished);
        q->add(&lib);
        q->add(&note1);
        q->add(&note2);
        m_cache->run(q);
        QVERIFY(finished.wait());
    }

    ItemsModel model;
    QSignalSpy countChanged(&model, &ItemsModel::countChanged);
    model.setTag("A Tag");
    model.setCache(m_cache);
    model.setParentItem(lib.uid());
    QVERIFY(countChanged.wait());
    QCOMPARE(model.count(), 1);
    QCOMPARE(model.data(model.index(0, 0), ItemsModel::ItemRole).value<Note*>()->uid(),
             note2.uid());
}

void ItemsModelTest::testFilterByNoTags()
{
    Library lib;
    Note note1, note2;
    note1.setLibraryId(lib.uid());
    note1.setTitle("Note 1");
    note2.setLibraryId(lib.uid());
    note2.setTitle("Note 2");
    note2.setTags({ "A Tag" });

    {
        auto q = new InsertOrUpdateItemsQuery;
        QSignalSpy finished(q, &InsertOrUpdateItemsQuery::finished);
        q->add(&lib);
        q->add(&note1);
        q->add(&note2);
        m_cache->run(q);
        QVERIFY(finished.wait());
    }

    ItemsModel model;
    QSignalSpy countChanged(&model, &ItemsModel::countChanged);
    model.setUntaggedOnly(true);
    model.setCache(m_cache);
    model.setParentItem(lib.uid());
    QVERIFY(countChanged.wait());
    QCOMPARE(model.count(), 1);
    QCOMPARE(model.data(model.index(0, 0), ItemsModel::ItemRole).value<Note*>()->uid(),
             note1.uid());
}

void ItemsModelTest::cleanup()
{
    delete m_cache;
    delete m_tmpDir;
}

QTEST_MAIN(ItemsModelTest)
#include "test_itemsmodel.moc"

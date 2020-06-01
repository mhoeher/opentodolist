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
#include "datastorage/cache.h"
#include "datastorage/insertorupdateitemsquery.h"
#include "models/librariesmodel.h"

class LibrariesModelTest : public QObject
{
    Q_OBJECT

private slots:

    void initTestCase() {}
    void init() {}
    void run();
    void cleanup() {}
    void cleanupTestCase() {}
};

void LibrariesModelTest::run()
{
    QTemporaryDir tmpDir;
    Cache cache;
    cache.setCacheDirectory(tmpDir.path());
    QVERIFY(cache.open());

    LibrariesModel model;
    QSignalSpy rowsInserted(&model, &LibrariesModel::rowsInserted);
    QSignalSpy dataChanged(&model, &LibrariesModel::dataChanged);
    model.setCache(&cache);
    QCOMPARE(model.rowCount(), 0);

    Library lib;
    lib.setName("Library 1");
    {
        auto q = new InsertOrUpdateItemsQuery;
        QSignalSpy finished(q, &InsertOrUpdateItemsQuery::finished);
        q->add(&lib);
        cache.run(q);
        QVERIFY(finished.wait());
    }

    QVERIFY(model.rowCount() == 1 || rowsInserted.wait());
    {
        auto idx = model.index(0);
        auto l = idx.data(LibrariesModel::LibraryRole).value<Library*>();
        QCOMPARE(l->name(), QString("Library 1"));
        QCOMPARE(l->uid(), lib.uid());
    }

    Library lib2;
    lib2.setName("Library 2");

    {
        auto q = new InsertOrUpdateItemsQuery;
        QSignalSpy finished(q, &InsertOrUpdateItemsQuery::finished);
        q->add(&lib2);
        cache.run(q);
        QVERIFY(finished.wait());
    }

    QVERIFY(model.rowCount() == 2 || rowsInserted.wait());
    {
        auto idx = model.index(0);
        auto l = idx.data(LibrariesModel::LibraryRole).value<Library*>();
        QCOMPARE(l->name(), QString("Library 1"));
        QCOMPARE(l->uid(), lib.uid());

        idx = model.index(1);
        l = idx.data(LibrariesModel::LibraryRole).value<Library*>();
        QCOMPARE(l->name(), QString("Library 2"));
        QCOMPARE(l->uid(), lib2.uid());
    }

    lib.setName("Changed Library");

    {
        auto q = new InsertOrUpdateItemsQuery;
        QSignalSpy finished(q, &InsertOrUpdateItemsQuery::finished);
        q->add(&lib);
        cache.run(q);
        QVERIFY(finished.wait());
    }

    QVERIFY(dataChanged.wait());

    {
        auto idx = model.index(0);
        auto l = idx.data(LibrariesModel::LibraryRole).value<Library*>();
        QCOMPARE(l->name(), QString("Changed Library"));
        QCOMPARE(l->uid(), lib.uid());

        idx = model.index(1);
        l = idx.data(LibrariesModel::LibraryRole).value<Library*>();
        QCOMPARE(l->name(), QString("Library 2"));
        QCOMPARE(l->uid(), lib2.uid());
    }
}

QTEST_MAIN(LibrariesModelTest)
#include "test_librariesmodel.moc"

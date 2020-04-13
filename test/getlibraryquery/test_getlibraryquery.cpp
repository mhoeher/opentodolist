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
#include <QRegularExpression>
#include <QSet>
#include <QSignalSpy>
#include <QTemporaryDir>
#include <QTest>

#include "datamodel/library.h"
#include "datastorage/cache.h"
#include "datastorage/getlibraryquery.h"
#include "datastorage/insertorupdateitemsquery.h"

class GetLibraryQueryTest : public QObject
{
    Q_OBJECT

private slots:

    void initTestCase() {}
    void init() {}
    void run();
    void cleanup() {}
    void cleanupTestCase() {}
};

void GetLibraryQueryTest::run()
{
    QTemporaryDir tmpDir;
    Cache cache;
    cache.setCacheDirectory(tmpDir.path());
    cache.setCacheSize(1024 * 1024);
    QVERIFY(cache.open());

    Library lib;
    lib.setName("A library");

    {
        auto q = new InsertOrUpdateItemsQuery;
        QSignalSpy finished(q, &InsertOrUpdateItemsQuery::finished);
        QSignalSpy destroyed(q, &InsertOrUpdateItemsQuery::destroyed);

        q->add(&lib);
        cache.run(q);

        QVERIFY(destroyed.wait());
        QCOMPARE(finished.count(), 1);
    }

    {
        auto q = new GetLibraryQuery();
        q->setUid(lib.uid());
        QSignalSpy libraryLoaded(q, &GetLibraryQuery::libraryLoaded);
        cache.run(q);
        QVERIFY(libraryLoaded.wait());
        QSharedPointer<Library> l(Library::decache(libraryLoaded.value(0).value(0)));
        QVERIFY(l != nullptr);
        QCOMPARE(l->uid(), lib.uid());
        QCOMPARE(l->name(), lib.name());
    }
}

QTEST_MAIN(GetLibraryQueryTest)
#include "test_getlibraryquery.moc"

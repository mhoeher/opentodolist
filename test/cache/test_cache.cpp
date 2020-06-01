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
#include <QTest>
#include <QTemporaryDir>

#include "datastorage/cache.h"

class CacheTest : public QObject
{
    Q_OBJECT

private slots:

    void initTestCase() {}
    void init();
    void constructor();
    void cacheDirectory();
    void open();
    void cleanup();
    void cleanupTestCase() {}

private:
    QTemporaryDir* tempDir;

    static const size_t CACHE_SIZE = 1024 * 1024;
};

void CacheTest::init()
{
    tempDir = new QTemporaryDir();
}

void CacheTest::constructor()
{
    auto c = new Cache();
    delete c;
}

void CacheTest::cacheDirectory()
{
    Cache c;
    c.setCacheDirectory(tempDir->path());
    QCOMPARE(c.cacheDirectory(), tempDir->path());
}

void CacheTest::open()
{
    {
        Cache c;
        QVERIFY(!c.isValid());
        QTest::ignoreMessage(QtWarningMsg,
                             "Failed to open cache directory \"\""
                             " : \"Empty path passed to "
                             "environment\"");
        QVERIFY(!c.open());
        QVERIFY(!c.isValid());
    }
    {
        Cache c;
        QVERIFY(!c.isValid());
        c.setCacheDirectory(tempDir->path());
        c.setCacheSize(CACHE_SIZE);
        //        QTest::ignoreMessage(QtDebugMsg,
        //                             "Cache is uninitialized - "
        //                             "initializing to Version 0 now");
        QVERIFY(c.open());
        QVERIFY(c.isValid());
    }
}

void CacheTest::cleanup()
{
    delete tempDir;
}

QTEST_MAIN(CacheTest)
#include "test_cache.moc"

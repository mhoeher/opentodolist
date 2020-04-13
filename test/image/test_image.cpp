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

#include "datamodel/image.h"

class ImageTest : public QObject
{
    Q_OBJECT

private slots:

    void initTestCase() {}
    void init();
    void testProperties();
    void testPersistence();
    void testSaveLoad();
    void cleanup();
    void cleanupTestCase() {}

private:
    QTemporaryDir *m_dir;
};

void ImageTest::init()
{
    m_dir = new QTemporaryDir();
}

void ImageTest::testProperties()
{
    Image item;
    QSignalSpy imageChanged(&item, &Image::imageChanged);

    item.setImage("hello.bmp");

    QCOMPARE(imageChanged.count(), 1);

    QCOMPARE(item.image(), QString("hello.bmp"));
}

void ImageTest::testPersistence()
{
    Image item, anotherItem;
    QSignalSpy imageChanged(&anotherItem, &Image::imageChanged);

    item.setImage("hello.bmp");

    anotherItem.fromVariant(item.toVariant());

    QCOMPARE(imageChanged.count(), 1);

    QCOMPARE(anotherItem.image(), item.image());
}

void ImageTest::testSaveLoad()
{
    Image item(QDir(m_dir->path()));
    QSignalSpy imageChanged(&item, &Image::imageChanged);
    item.setImage(SRCDIR "/test_image.cpp");
    QVERIFY(item.validImage());
    QVERIFY(item.save());

    Image anotherItem(item.filename());
    anotherItem.load();
    QCOMPARE(anotherItem.image(), item.image());
    QVERIFY(anotherItem.validImage());
}

void ImageTest::cleanup()
{
    delete m_dir;
}

QTEST_MAIN(ImageTest)
#include "test_image.moc"

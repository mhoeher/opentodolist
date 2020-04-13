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

#include <QTemporaryDir>
#include <QObject>
#include <QSignalSpy>
#include <QTest>

#include "datamodel/complexitem.h"

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
    QCOMPARE(item.attachments(), QStringList({ "foo.txt" }));
    QVERIFY(QFile(item.attachmentFileName("foo.txt")).exists());

    item.attachFile(d.absoluteFilePath("res/foo.txt"));
    QCOMPARE(attachmentsChanged.count(), 2);
    QCOMPARE(item.attachments(), QStringList({ "foo-1.txt", "foo.txt" }));
    QVERIFY(QFile(item.attachmentFileName("foo-1.txt")).exists());

    item.detachFile("foo.txt");
    QCOMPARE(attachmentsChanged.count(), 3);
    QCOMPARE(item.attachments(), QStringList({ "foo-1.txt" }));
    QVERIFY(QFile(item.attachmentFileName("foo-1.txt")).exists());
    QVERIFY(!QFile(item.attachmentFileName("foo.txt")).exists());
    item.save();

    auto attachmentPath = item.attachmentFileName("foo-1.txt");
    QVERIFY(QFile(attachmentPath).exists());
    QVERIFY(item.deleteItem());
    QVERIFY(!QFile(attachmentPath).exists());
}

QTEST_MAIN(ComplexItemTest)
#include "test_complexitem.moc"

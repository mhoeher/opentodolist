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
#include <QTemporaryDir>
#include <QTest>
#include <QSignalSpy>

#include "datamodel/notepage.h"

class NotePageTest : public QObject
{
    Q_OBJECT

private slots:

    void initTestCase() {}
    void init() {}
    void persistence();
    void cleanup() {}
    void cleanupTestCase() {}
};

void NotePageTest::persistence()
{
    QTemporaryDir tmpDir;
    QString filename;
    auto noteUid = QUuid::createUuid();
    {
        NotePage page(QDir(tmpDir.path()));
        QSignalSpy spy(&page, &NotePage::changed);
        page.setNotes("Hello World!");
        QCOMPARE(spy.count(), 1);
        page.setNoteUid(noteUid);
        QCOMPARE(spy.count(), 2);
        QVERIFY(page.save());
        filename = page.filename();
    }
    {
        NotePage page(filename);
        QVERIFY(page.load());
        QCOMPARE(page.notes(), "Hello World!");
        QCOMPARE(page.noteUid(), noteUid);
    }
}

QTEST_MAIN(NotePageTest)
#include "test_notepage.moc"

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
#include <QSignalSpy>
#include <QTemporaryDir>
#include <QTest>

#include "datamodel/image.h"
#include "datamodel/library.h"
#include "datamodel/note.h"
#include "datamodel/task.h"
#include "datamodel/todo.h"
#include "datamodel/todolist.h"
#include "datastorage/cache.h"
#include "datastorage/insertorupdateitemsquery.h"
#include "datastorage/librariesitemsquery.h"

class LibrariesItemsQueryTest : public QObject
{
    Q_OBJECT

private slots:

    void initTestCase() {}
    void init() {}
    void run();
    void cleanup() {}
    void cleanupTestCase() {}
};

void LibrariesItemsQueryTest::run()
{
    //    QTest::ignoreMessage(QtDebugMsg,
    //                         QRegularExpression(".*Cache is uninitialized.*"));
    QTemporaryDir tmpDir;
    Cache cache;
    cache.setCacheDirectory(tmpDir.path());
    cache.setCacheSize(1024 * 1024);
    QVERIFY(cache.open());

    {
        auto q = new LibrariesItemsQuery();
        QSignalSpy librariesAvailable(q, &LibrariesItemsQuery::librariesAvailable);
        QSignalSpy destroyed(q, &LibrariesItemsQuery::destroyed);
        cache.run(q);
        QVERIFY(destroyed.wait());
        QCOMPARE(librariesAvailable.count(), 1);
        QCOMPARE(librariesAvailable.at(0).at(0).toList().count(), 0);
    }

    Library lib;
    TodoList todoList;
    todoList.setTitle("A todo list");
    todoList.setLibraryId(lib.uid());
    todoList.setTags({ "Tag 1" });
    Todo todo;
    todo.setTitle("A todo");
    todo.setTodoListUid(todoList.uid());
    Task task;
    task.setTitle("A task");
    task.setTodoUid(todo.uid());
    Note note;
    note.setTitle("A note");
    note.setLibraryId(lib.uid());
    note.setTags({ "Tag 2" });
    Image image;
    image.setTitle("An image");
    image.setLibraryId(lib.uid());
    image.setTags({ "Tag 3" });

    {
        auto q = new InsertOrUpdateItemsQuery;
        QSignalSpy finished(q, &InsertOrUpdateItemsQuery::finished);
        QSignalSpy destroyed(q, &InsertOrUpdateItemsQuery::destroyed);

        q->add(&lib);
        q->add(&todoList);
        q->add(&todo);
        q->add(&task);
        q->add(&note);
        q->add(&image);
        cache.run(q);

        QVERIFY(destroyed.wait());
        QCOMPARE(finished.count(), 1);
    }

    {
        auto q = new LibrariesItemsQuery();
        q->setIncludeCalculatedValues(true);
        QSignalSpy librariesAvailable(q, &LibrariesItemsQuery::librariesAvailable);
        QSignalSpy destroyed(q, &LibrariesItemsQuery::destroyed);
        cache.run(q);
        QVERIFY(destroyed.wait());
        QCOMPARE(librariesAvailable.count(), 1);
        auto libraries = librariesAvailable.at(0).at(0).toList();
        QCOMPARE(libraries.count(), 1);
        auto entry = libraries.at(0).value<LibraryCacheEntry>();
        QVERIFY(entry.valid);
        auto eEntry = lib.encache();
        QCOMPARE(entry, lib.encache());

        // https://gitlab.com/rpdev/opentodolist/-/issues/616: Check if we properly read
        // back tags for libraries:
        auto lib2 = Library::decache(entry);
        QVERIFY(lib2 != nullptr);
        auto tags = lib2->tags();
        QCOMPARE(tags.length(), 3);
        QVERIFY(tags.contains("Tag 1"));
        QVERIFY(tags.contains("Tag 2"));
        QVERIFY(tags.contains("Tag 3"));
        delete lib2;
    }

    {
        lib.setName("Changed library title");
        todoList.setTitle("Changed todo list");
        todo.setTitle("Changed todo");
        task.setTitle("Changed task");
        note.setTitle("Changed note");
        image.setTitle("Changed image");

        auto q = new InsertOrUpdateItemsQuery;
        QSignalSpy finished(q, &InsertOrUpdateItemsQuery::finished);
        QSignalSpy destroyed(q, &InsertOrUpdateItemsQuery::destroyed);

        q->add(&lib);
        q->add(&todoList);
        q->add(&todo);
        q->add(&task);
        q->add(&note);
        q->add(&image);
        cache.run(q);

        QVERIFY(destroyed.wait());
        QCOMPARE(finished.count(), 1);
    }

    {
        auto q = new LibrariesItemsQuery();
        QSignalSpy librariesAvailable(q, &LibrariesItemsQuery::librariesAvailable);
        QSignalSpy destroyed(q, &LibrariesItemsQuery::destroyed);
        cache.run(q);
        QVERIFY(destroyed.wait());
        QCOMPARE(librariesAvailable.count(), 1);
        auto libraries = librariesAvailable.at(0).at(0).toList();
        QCOMPARE(libraries.count(), 1);
        auto entry = libraries.at(0).value<LibraryCacheEntry>();
        QVERIFY(entry.valid);
        auto eEntry = lib.encache();
        QCOMPARE(entry, lib.encache());
    }
}

QTEST_MAIN(LibrariesItemsQueryTest)
#include "test_librariesitemsquery.moc"

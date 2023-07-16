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
#include "datastorage/getitemquery.h"

class InsertOrUpdateItemsQueryTest : public QObject
{
    Q_OBJECT

private slots:

    void initTestCase() {}
    void init() {}
    void insertOrUpdate();
    void recurringSubTasks();
    void cleanup() {}
    void cleanupTestCase() {}
};

void InsertOrUpdateItemsQueryTest::insertOrUpdate()
{
    //    QTest::ignoreMessage(QtDebugMsg,
    //                         QRegularExpression(".*Cache is uninitialized.*"));
    QTemporaryDir tmpDir;
    Cache cache;
    cache.setCacheDirectory(tmpDir.path());
    cache.setCacheSize(1024 * 1024);
    QVERIFY(cache.open());

    Library lib;
    TodoList todoList;
    todoList.setTitle("A todo list");
    todoList.setLibraryId(lib.uid());
    Todo todo;
    todo.setTitle("A todo");
    todo.setTodoListUid(todoList.uid());
    Task task;
    task.setTitle("A task");
    task.setTodoUid(todo.uid());
    Note note;
    note.setTitle("A note");
    note.setLibraryId(lib.uid());
    Image image;
    image.setTitle("An image");
    image.setLibraryId(lib.uid());

    {
        auto q = new InsertOrUpdateItemsQuery;
        QSignalSpy finished(q, &InsertOrUpdateItemsQuery::finished);
        QSignalSpy cacheFinished(&cache, &Cache::finished);
        QSignalSpy dataChanged(&cache, &Cache::dataChanged);
        QSignalSpy librariesChanged(&cache, &Cache::librariesChanged);

        q->add(&lib);
        q->add(&todoList);
        q->add(&todo);
        q->add(&task);
        q->add(&note);
        q->add(&image);
        cache.run(q);

        QVERIFY(cacheFinished.wait());
        QCOMPARE(finished.count(), 1);
        QCOMPARE(dataChanged.count(), 1);
        QCOMPARE(librariesChanged.count(), 1);
        QCOMPARE(librariesChanged.at(0).at(0).toList().at(0).toUuid(), lib.uid());
    }

    {
        auto q = new InsertOrUpdateItemsQuery;
        QSignalSpy finished(q, &InsertOrUpdateItemsQuery::finished);
        QSignalSpy cacheFinished(&cache, &Cache::finished);
        QSignalSpy dataChanged(&cache, &Cache::dataChanged);
        QSignalSpy librariesChanged(&cache, &Cache::librariesChanged);

        q->add(&lib);
        q->add(&todoList);
        q->add(&todo);
        q->add(&task);
        q->add(&note);
        q->add(&image);
        cache.run(q);

        QVERIFY(cacheFinished.wait());
        QCOMPARE(finished.count(), 1);
        QCOMPARE(dataChanged.count(), 0);
        QCOMPARE(librariesChanged.count(), 0);
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
        QSignalSpy cacheFinished(&cache, &Cache::finished);
        QSignalSpy dataChanged(&cache, &Cache::dataChanged);

        q->add(&lib);
        q->add(&todoList);
        q->add(&todo);
        q->add(&task);
        q->add(&note);
        q->add(&image);
        cache.run(q);

        QVERIFY(cacheFinished.wait());
        QCOMPARE(finished.count(), 1);
        QCOMPARE(dataChanged.count(), 1);
    }
}

void InsertOrUpdateItemsQueryTest::recurringSubTasks()
{
    QTemporaryDir tmpDir;
    Cache cache;
    cache.setCacheDirectory(tmpDir.path());
    cache.setCacheSize(1024 * 1024);
    QVERIFY(cache.open());

    Library lib;
    TodoList todoList;
    todoList.setTitle("A todo list");
    todoList.setLibraryId(lib.uid());
    todoList.setDueTo(QDateTime::currentDateTime());
    todoList.setRecurrencePattern(ComplexItem::RecurrencePattern::RecurDaily);
    QVERIFY(todoList.isRecurring());
    Todo todo;
    todo.setTitle("A todo");
    todo.setDone(true);
    todo.setTodoListUid(todoList.uid());
    Task task;
    task.setTitle("A task");
    task.setDone(true);
    task.setTodoUid(todo.uid());

    {
        auto q = new InsertOrUpdateItemsQuery;
        QSignalSpy finished(q, &InsertOrUpdateItemsQuery::finished);
        QSignalSpy cacheFinished(&cache, &Cache::finished);
        QSignalSpy dataChanged(&cache, &Cache::dataChanged);
        QSignalSpy librariesChanged(&cache, &Cache::librariesChanged);

        q->add(&lib);
        q->add(&todoList);
        q->add(&todo);
        q->add(&task);
        cache.run(q);

        QVERIFY(cacheFinished.wait());
        QCOMPARE(finished.count(), 1);
        QCOMPARE(dataChanged.count(), 1);
        QCOMPARE(librariesChanged.count(), 1);
        QCOMPARE(librariesChanged.at(0).at(0).toList().at(0).toUuid(), lib.uid());
    }

    // Mark current occurrence of the todo list as done. This should - upon saving - "un-do"
    // any contained todos and tasks:
    todoList.markCurrentOccurrenceAsDone();
    {
        auto q = new InsertOrUpdateItemsQuery;
        QSignalSpy finished(q, &InsertOrUpdateItemsQuery::finished);

        q->add(&todoList);
        cache.run(q);

        QVERIFY(finished.wait());
    }

    // Check if the todo is undone again:
    {
        auto q = new GetItemQuery;
        q->setUid(todo.uid());
        QSignalSpy itemLoaded(q, &GetItemQuery::itemLoaded);
        cache.run(q);
        QVERIFY(itemLoaded.wait());
        ItemPtr item(Item::decache(itemLoaded.value(0).value(0)));
        auto todo_ = item.objectCast<Todo>();
        QVERIFY(todo_);
        QVERIFY(!todo_->done());
    }

    // Check if the task is undone again:
    {
        auto q = new GetItemQuery;
        q->setUid(task.uid());
        QSignalSpy itemLoaded(q, &GetItemQuery::itemLoaded);
        cache.run(q);
        QVERIFY(itemLoaded.wait());
        ItemPtr item(Item::decache(itemLoaded.value(0).value(0)));
        auto task_ = item.objectCast<Task>();
        QVERIFY(task_);
        QVERIFY(!task_->done());
    }
}

QTEST_MAIN(InsertOrUpdateItemsQueryTest)
#include "test_insertorupdateitemsquery.moc"

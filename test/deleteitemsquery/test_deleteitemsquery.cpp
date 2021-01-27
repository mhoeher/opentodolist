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

#include "datamodel/image.h"
#include "datamodel/library.h"
#include "datamodel/note.h"
#include "datamodel/task.h"
#include "datamodel/todo.h"
#include "datamodel/todolist.h"
#include "datastorage/cache.h"
#include "datastorage/deleteitemsquery.h"
#include "datastorage/getitemsquery.h"
#include "datastorage/insertorupdateitemsquery.h"

class DeleteItemsQueryTest : public QObject
{
    Q_OBJECT

private slots:

    void initTestCase() {}
    void init() {}
    void run();
    void deleteChildren();
    void cleanup() {}
    void cleanupTestCase() {}
};

void DeleteItemsQueryTest::run()
{
    //    QTest::ignoreMessage(QtDebugMsg,
    //                         QRegularExpression(".*Cache is uninitialized.*"));
    QTemporaryDir tmpDir;
    Cache cache;
    cache.setCacheDirectory(tmpDir.path());
    cache.setCacheSize(1024 * 1024);
    QVERIFY(cache.open());

    {
        auto q = new DeleteItemsQuery();
        QSignalSpy itemDeleted(q, &DeleteItemsQuery::itemDeleted);
        QSignalSpy destroyed(q, &GetItemsQuery::destroyed);
        cache.run(q);
        QVERIFY(destroyed.wait());
        QCOMPARE(itemDeleted.count(), 0);
    }

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

        q->add(&lib);
        q->add(&todoList);
        q->add(&todo);
        q->add(&task);
        q->add(&note);
        q->add(&image);
        cache.run(q);

        QVERIFY(cacheFinished.wait());
        QCOMPARE(finished.count(), 1);
    }

    {
        auto q = new GetItemsQuery();
        QSignalSpy itemsAvailable(q, &GetItemsQuery::itemsAvailable);
        QSignalSpy destroyed(q, &GetItemsQuery::destroyed);
        cache.run(q);
        QVERIFY(destroyed.wait());
        QCOMPARE(itemsAvailable.count(), 1);
        auto items = itemsAvailable.at(0).at(0).toList();
        QCOMPARE(items.count(), 5);
        QSet<QByteArray> got = QSet<QByteArray>({
                items.at(0).value<ItemCacheEntry>().toJson(),
                items.at(1).value<ItemCacheEntry>().toJson(),
                items.at(2).value<ItemCacheEntry>().toJson(),
                items.at(3).value<ItemCacheEntry>().toJson(),
                items.at(4).value<ItemCacheEntry>().toJson(),
        });
        QSet<QByteArray> expected = QSet<QByteArray>({
                todoList.encache().toJson(),
                todo.encache().toJson(),
                task.encache().toJson(),
                note.encache().toJson(),
                image.encache().toJson(),
        });
        QCOMPARE(got, expected);
    }

    {
        auto q = new DeleteItemsQuery();
        q->deleteItem(&task);
        QSignalSpy itemDeleted(q, &DeleteItemsQuery::itemDeleted);
        QSignalSpy cacheFinished(&cache, &Cache::finished);
        QSignalSpy dataChanged(&cache, &Cache::dataChanged);
        QSignalSpy librariesChanged(&cache, &Cache::librariesChanged);
        cache.run(q);
        QVERIFY(cacheFinished.wait());
        QCOMPARE(dataChanged.count(), 1);
        QCOMPARE(librariesChanged.count(), 1);
        QCOMPARE(librariesChanged.at(0).at(0).toList().at(0).toUuid(), lib.uid());
    }

    {
        auto q = new DeleteItemsQuery();
        q->deleteItem(&task);
        QSignalSpy itemDeleted(q, &DeleteItemsQuery::itemDeleted);
        QSignalSpy cacheFinished(&cache, &Cache::finished);
        QSignalSpy dataChanged(&cache, &Cache::dataChanged);
        QSignalSpy librariesChanged(&cache, &Cache::librariesChanged);
        cache.run(q);
        QVERIFY(cacheFinished.wait());
        QCOMPARE(dataChanged.count(), 0);
        QCOMPARE(librariesChanged.count(), 0);
    }

    {
        auto q = new GetItemsQuery();
        QSignalSpy itemsAvailable(q, &GetItemsQuery::itemsAvailable);
        QSignalSpy destroyed(q, &GetItemsQuery::destroyed);
        cache.run(q);
        QVERIFY(destroyed.wait());
        QCOMPARE(itemsAvailable.count(), 1);
        auto items = itemsAvailable.at(0).at(0).toList();
        QCOMPARE(items.count(), 4);
        QSet<QByteArray> got = QSet<QByteArray>({
                items.at(0).value<ItemCacheEntry>().toJson(),
                items.at(1).value<ItemCacheEntry>().toJson(),
                items.at(2).value<ItemCacheEntry>().toJson(),
                items.at(3).value<ItemCacheEntry>().toJson(),
        });
        QSet<QByteArray> expected = QSet<QByteArray>({
                todoList.encache().toJson(),
                todo.encache().toJson(),
                note.encache().toJson(),
                image.encache().toJson(),
        });
        QCOMPARE(got, expected);
    }

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
        auto q = new DeleteItemsQuery();
        q->deleteItem(&todo);
        QSignalSpy cacheFinished(&cache, &Cache::finished);
        QSignalSpy dataChanged(&cache, &Cache::dataChanged);
        QSignalSpy itemDeleted(q, &DeleteItemsQuery::itemDeleted);
        QSignalSpy destroyed(q, &DeleteItemsQuery::destroyed);
        cache.run(q);
        QVERIFY(cacheFinished.wait());
        QCOMPARE(dataChanged.count(), 1);
        if (destroyed.count() == 0) {
            QVERIFY(destroyed.wait());
        }
    }

    {
        auto q = new GetItemsQuery();
        QSignalSpy itemsAvailable(q, &GetItemsQuery::itemsAvailable);
        QSignalSpy destroyed(q, &GetItemsQuery::destroyed);
        cache.run(q);
        QVERIFY(destroyed.wait());
        QCOMPARE(itemsAvailable.count(), 1);
        auto items = itemsAvailable.at(0).at(0).toList();
        QCOMPARE(items.count(), 3);
        QSet<QByteArray> got = QSet<QByteArray>({
                items.at(0).value<ItemCacheEntry>().toJson(),
                items.at(1).value<ItemCacheEntry>().toJson(),
                items.at(2).value<ItemCacheEntry>().toJson(),
        });
        QSet<QByteArray> expected = QSet<QByteArray>({
                todoList.encache().toJson(),
                note.encache().toJson(),
                image.encache().toJson(),
        });
        QCOMPARE(got, expected);
    }

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
        auto q = new DeleteItemsQuery();
        q->deleteItem(&todoList);
        QSignalSpy itemDeleted(q, &DeleteItemsQuery::itemDeleted);
        QSignalSpy destroyed(q, &DeleteItemsQuery::destroyed);
        QSignalSpy cacheFinished(&cache, &Cache::finished);
        QSignalSpy dataChanged(&cache, &Cache::dataChanged);
        cache.run(q);
        QVERIFY(cacheFinished.wait());
        QCOMPARE(dataChanged.count(), 1);
        if (destroyed.count() == 0) {
            QVERIFY(destroyed.wait());
        }
    }

    {
        auto q = new GetItemsQuery();
        QSignalSpy itemsAvailable(q, &GetItemsQuery::itemsAvailable);
        QSignalSpy destroyed(q, &GetItemsQuery::destroyed);
        cache.run(q);
        QVERIFY(destroyed.wait());
        QCOMPARE(itemsAvailable.count(), 1);
        auto items = itemsAvailable.at(0).at(0).toList();
        QCOMPARE(items.count(), 2);
        QSet<QByteArray> got = QSet<QByteArray>({
                items.at(0).value<ItemCacheEntry>().toJson(),
                items.at(1).value<ItemCacheEntry>().toJson(),
        });
        QSet<QByteArray> expected = QSet<QByteArray>({
                note.encache().toJson(),
                image.encache().toJson(),
        });
        QCOMPARE(got, expected);
    }

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
        auto q = new DeleteItemsQuery();
        q->deleteLibrary(&lib, false);
        QSignalSpy itemDeleted(q, &DeleteItemsQuery::itemDeleted);
        QSignalSpy destroyed(q, &DeleteItemsQuery::destroyed);
        QSignalSpy cacheFinished(&cache, &Cache::finished);
        QSignalSpy dataChanged(&cache, &Cache::dataChanged);
        cache.run(q);
        QVERIFY(cacheFinished.wait());
        QCOMPARE(dataChanged.count(), 1);
        if (destroyed.count() == 0) {
            QVERIFY(destroyed.wait());
        }
    }

    {
        auto q = new GetItemsQuery();
        QSignalSpy itemsAvailable(q, &GetItemsQuery::itemsAvailable);
        QSignalSpy destroyed(q, &GetItemsQuery::destroyed);
        cache.run(q);
        QVERIFY(destroyed.wait());
        QCOMPARE(itemsAvailable.count(), 1);
        auto items = itemsAvailable.at(0).at(0).toList();
        QCOMPARE(items.count(), 0);
    }
}

void DeleteItemsQueryTest::deleteChildren()
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
    Todo todo1;
    todo1.setTitle("A todo 1");
    todo1.setDone(false);
    todo1.setTodoListUid(todoList.uid());
    Task task1;
    task1.setTitle("Task 1");
    task1.setTodoUid(todo1.uid());
    Todo todo2;
    todo2.setTitle("A todo 2");
    todo2.setDone(true);
    todo2.setTodoListUid(todoList.uid());
    Task task2;
    task2.setTitle("Task 2");
    task2.setTodoUid(todo2.uid());
    Todo todo3;
    todo3.setTitle("A todo 3");
    todo3.setDone(false);
    todo3.setTodoListUid(todoList.uid());
    Todo todo4;
    todo4.setTitle("A todo 4");
    todo4.setDone(true);
    todo4.setTodoListUid(todoList.uid());
    Todo todo5;
    todo5.setTitle("A todo 5");
    todo5.setDone(false);
    todo5.setTodoListUid(todoList.uid());
    Todo todo6;
    todo6.setTitle("A todo 6");
    todo6.setDone(true);
    todo6.setTodoListUid(todoList.uid());
    Note note;
    note.setTitle("A note");
    note.setLibraryId(lib.uid());

    {
        auto q = new InsertOrUpdateItemsQuery;
        QSignalSpy finished(q, &InsertOrUpdateItemsQuery::finished);
        QSignalSpy cacheFinished(&cache, &Cache::finished);

        q->add(&lib);
        q->add(&todoList);
        q->add(&todo1);
        q->add(&task1);
        q->add(&todo2);
        q->add(&task2);
        q->add(&todo3);
        q->add(&todo4);
        q->add(&todo5);
        q->add(&todo6);
        q->add(&note);
        cache.run(q);

        QVERIFY(cacheFinished.wait());
        QCOMPARE(finished.count(), 1);
    }

    {
        auto q = new DeleteItemsQuery();
        QSignalSpy itemDeleted(q, &DeleteItemsQuery::itemDeleted);
        QSignalSpy destroyed(q, &GetItemsQuery::destroyed);
        q->deleteChildrenOfItem(&todoList, [](const Item* item) {
            auto todo = qobject_cast<const Todo*>(item);
            return todo && todo->done();
        });
        cache.run(q);
        QVERIFY(destroyed.wait());
        QCOMPARE(itemDeleted.count(), 1);
    }

    {
        auto q = new GetItemsQuery();
        QSignalSpy itemsAvailable(q, &GetItemsQuery::itemsAvailable);
        QSignalSpy destroyed(q, &GetItemsQuery::destroyed);
        cache.run(q);
        QVERIFY(destroyed.wait());
        QCOMPARE(itemsAvailable.count(), 1);
        auto items = itemsAvailable.at(0).at(0).toList();
        QCOMPARE(items.count(), 6);
        QSet<QByteArray> got = QSet<QByteArray>({
                items.at(0).value<ItemCacheEntry>().toJson(),
                items.at(1).value<ItemCacheEntry>().toJson(),
                items.at(2).value<ItemCacheEntry>().toJson(),
                items.at(3).value<ItemCacheEntry>().toJson(),
                items.at(4).value<ItemCacheEntry>().toJson(),
                items.at(5).value<ItemCacheEntry>().toJson(),
        });
        QSet<QByteArray> expected = QSet<QByteArray>({
                todoList.encache().toJson(),
                todo1.encache().toJson(),
                task1.encache().toJson(),
                todo3.encache().toJson(),
                todo5.encache().toJson(),
                note.encache().toJson(),
        });
        QCOMPARE(got, expected);
    }
}

QTEST_MAIN(DeleteItemsQueryTest)
#include "test_deleteitemsquery.moc"

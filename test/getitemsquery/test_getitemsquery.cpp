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
#include <QVariant>

#include "datamodel/image.h"
#include "datamodel/library.h"
#include "datamodel/note.h"
#include "datamodel/task.h"
#include "datamodel/todo.h"
#include "datamodel/todolist.h"
#include "datastorage/cache.h"
#include "datastorage/getitemsquery.h"
#include "datastorage/insertorupdateitemsquery.h"

class GetItemsQueryTest : public QObject
{
    Q_OBJECT

private slots:

    void initTestCase() {}
    void init() {}
    void run();
    void todoCalculatedProperties();
    void cleanup() {}
    void cleanupTestCase() {}
};

void GetItemsQueryTest::run()
{
    QTemporaryDir tmpDir;
    Cache cache;
    cache.setCacheDirectory(tmpDir.path());
    cache.setCacheSize(1024 * 1024);
    QVERIFY(cache.open());

    {
        auto q = new GetItemsQuery();
        QSignalSpy itemsAvailable(q, &GetItemsQuery::itemsAvailable);
        QSignalSpy destroyed(q, &GetItemsQuery::destroyed);
        cache.run(q);
        QVERIFY(destroyed.wait());
        QCOMPARE(itemsAvailable.count(), 1);
        QCOMPARE(itemsAvailable.at(0).at(0).toList().count(), 0);
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
        auto q = new GetItemsQuery();
        q->setParent(lib.uid());
        QSignalSpy itemsAvailable(q, &GetItemsQuery::itemsAvailable);
        QSignalSpy destroyed(q, &GetItemsQuery::destroyed);
        cache.run(q);
        QVERIFY(destroyed.wait());
        QCOMPARE(itemsAvailable.count(), 1);
        auto items = itemsAvailable.at(0).at(0).toList();
        QCOMPARE(items.count(), 3);
        QSet<QByteArray> got = QSet<QByteArray>({ items.at(0).value<ItemCacheEntry>().toJson(),
                                                  items.at(1).value<ItemCacheEntry>().toJson(),
                                                  items.at(2).value<ItemCacheEntry>().toJson() });
        QSet<QByteArray> expected = QSet<QByteArray>({
                todoList.encache().toJson(),
                note.encache().toJson(),
                image.encache().toJson(),
        });
        QCOMPARE(got, expected);
    }

    {
        auto q = new GetItemsQuery();
        q->setParent(todoList.uid());
        QSignalSpy itemsAvailable(q, &GetItemsQuery::itemsAvailable);
        QSignalSpy destroyed(q, &GetItemsQuery::destroyed);
        cache.run(q);
        QVERIFY(destroyed.wait());
        QCOMPARE(itemsAvailable.count(), 1);
        auto items = itemsAvailable.at(0).at(0).toList();
        QCOMPARE(items.count(), 1);
        QSet<QByteArray> got = QSet<QByteArray>({ items.at(0).value<ItemCacheEntry>().toJson() });
        QSet<QByteArray> expected = QSet<QByteArray>({ todo.encache().toJson() });
        QCOMPARE(got, expected);
    }

    {
        auto q = new GetItemsQuery();
        q->setParent(todoList.uid());
        q->setRecursive(true);
        QSignalSpy itemsAvailable(q, &GetItemsQuery::itemsAvailable);
        QSignalSpy destroyed(q, &GetItemsQuery::destroyed);
        cache.run(q);
        QVERIFY(destroyed.wait());
        QCOMPARE(itemsAvailable.count(), 1);
        auto items = itemsAvailable.at(0).at(0).toList();
        QCOMPARE(items.count(), 2);
        QSet<QByteArray> got = QSet<QByteArray>({ items.at(0).value<ItemCacheEntry>().toJson(),
                                                  items.at(1).value<ItemCacheEntry>().toJson() });
        QSet<QByteArray> expected =
                QSet<QByteArray>({ todo.encache().toJson(), task.encache().toJson() });
        QCOMPARE(got, expected);
    }

    {
        auto q = new GetItemsQuery();
        q->setParent(todo.uid());
        QSignalSpy itemsAvailable(q, &GetItemsQuery::itemsAvailable);
        QSignalSpy destroyed(q, &GetItemsQuery::destroyed);
        cache.run(q);
        QVERIFY(destroyed.wait());
        QCOMPARE(itemsAvailable.count(), 1);
        auto items = itemsAvailable.at(0).at(0).toList();
        QCOMPARE(items.count(), 1);
        QSet<QByteArray> got = QSet<QByteArray>({ items.at(0).value<ItemCacheEntry>().toJson() });
        QSet<QByteArray> expected = QSet<QByteArray>({ task.encache().toJson() });
        QCOMPARE(got, expected);
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
        auto q = new GetItemsQuery();
        q->setParent(lib.uid());
        QSignalSpy itemsAvailable(q, &GetItemsQuery::itemsAvailable);
        QSignalSpy destroyed(q, &GetItemsQuery::destroyed);
        cache.run(q);
        QVERIFY(destroyed.wait());
        QCOMPARE(itemsAvailable.count(), 1);
        auto items = itemsAvailable.at(0).at(0).toList();
        QCOMPARE(items.count(), 3);
        QSet<QByteArray> got = QSet<QByteArray>({ items.at(0).value<ItemCacheEntry>().toJson(),
                                                  items.at(1).value<ItemCacheEntry>().toJson(),
                                                  items.at(2).value<ItemCacheEntry>().toJson() });
        QSet<QByteArray> expected = QSet<QByteArray>({
                todoList.encache().toJson(),
                note.encache().toJson(),
                image.encache().toJson(),
        });
        QCOMPARE(got, expected);
    }

    {
        auto q = new GetItemsQuery();
        q->setParent(todoList.uid());
        QSignalSpy itemsAvailable(q, &GetItemsQuery::itemsAvailable);
        QSignalSpy destroyed(q, &GetItemsQuery::destroyed);
        cache.run(q);
        QVERIFY(destroyed.wait());
        QCOMPARE(itemsAvailable.count(), 1);
        auto items = itemsAvailable.at(0).at(0).toList();
        QCOMPARE(items.count(), 1);
        QSet<QByteArray> got = QSet<QByteArray>({ items.at(0).value<ItemCacheEntry>().toJson() });
        QSet<QByteArray> expected = QSet<QByteArray>({ todo.encache().toJson() });
        QCOMPARE(got, expected);
    }

    {
        auto q = new GetItemsQuery();
        q->setParent(todo.uid());
        QSignalSpy itemsAvailable(q, &GetItemsQuery::itemsAvailable);
        QSignalSpy destroyed(q, &GetItemsQuery::destroyed);
        cache.run(q);
        QVERIFY(destroyed.wait());
        QCOMPARE(itemsAvailable.count(), 1);
        auto items = itemsAvailable.at(0).at(0).toList();
        QCOMPARE(items.count(), 1);
        QSet<QByteArray> got = QSet<QByteArray>({ items.at(0).value<ItemCacheEntry>().toJson() });
        QSet<QByteArray> expected = QSet<QByteArray>({ task.encache().toJson() });
        QCOMPARE(got, expected);
    }

    {
        auto q = new GetItemsQuery();
        q->setParent(todoList.uid());
        q->setItemFilter([=](ItemPtr item, GetItemsQuery* query) {
            for (const auto& i : query->childrenOf(item->uid())) {
                if (i->title().contains("task", Qt::CaseInsensitive)) {
                    return true;
                }
            }
            return false;
        });
        QSignalSpy itemsAvailable(q, &GetItemsQuery::itemsAvailable);
        QSignalSpy destroyed(q, &GetItemsQuery::destroyed);
        cache.run(q);
        QVERIFY(destroyed.wait());
        QCOMPARE(itemsAvailable.count(), 1);
        auto items = itemsAvailable.at(0).at(0).toList();
        QCOMPARE(items.count(), 1);
        QSet<QByteArray> got = QSet<QByteArray>({ items.at(0).value<ItemCacheEntry>().toJson() });
        QSet<QByteArray> expected = QSet<QByteArray>({ todo.encache().toJson() });
        QCOMPARE(got, expected);
    }

    {
        auto q = new GetItemsQuery();
        q->setParent(todoList.uid());
        q->setItemFilter([=](ItemPtr, GetItemsQuery*) { return false; });
        QSignalSpy itemsAvailable(q, &GetItemsQuery::itemsAvailable);
        QSignalSpy destroyed(q, &GetItemsQuery::destroyed);
        cache.run(q);
        QVERIFY(destroyed.wait());
        QCOMPARE(itemsAvailable.count(), 1);
        auto items = itemsAvailable.at(0).at(0).toList();
        QCOMPARE(items.count(), 0);
    }
}

void GetItemsQueryTest::todoCalculatedProperties()
{
    QTemporaryDir tmpDir;
    Cache cache;
    cache.setCacheDirectory(tmpDir.path());
    cache.setCacheSize(1024 * 1024);
    QVERIFY(cache.open());

    Library lib;
    TodoList todoList;
    Todo todo;
    Task task1, task2;
    todoList.setLibraryId(lib.uid());
    todo.setTodoListUid(todoList.uid());
    task1.setTodoUid(todo.uid());
    task2.setTodoUid(todo.uid());

    {
        auto q = new InsertOrUpdateItemsQuery;
        QSignalSpy finished(q, &InsertOrUpdateItemsQuery::finished);
        q->add(&lib);
        q->add(&todoList);
        q->add(&todo);
        q->add(&task1);
        q->add(&task2);
        cache.run(q);
        QVERIFY(finished.wait());
    }

    {
        auto q = new GetItemsQuery;
        QSignalSpy readItems(q, &GetItemsQuery::itemsAvailable);
        q->setParent(todoList.uid());
        cache.run(q);
        QVERIFY(readItems.wait());
        auto todo = Item::decache(readItems.at(0).at(0).toList().at(0), this);
        QCOMPARE(todo->property("percentageDone").toInt(), 0);
        QCOMPARE(todo->property("numSubtasks").toInt(), 2);
        QCOMPARE(todo->property("numDoneSubtasks").toInt(), 0);
    }

    task1.setDone(true);

    {
        auto q = new InsertOrUpdateItemsQuery;
        QSignalSpy finished(q, &InsertOrUpdateItemsQuery::finished);
        q->add(&task1);
        cache.run(q);
        QVERIFY(finished.wait());
    }

    {
        auto q = new GetItemsQuery;
        QSignalSpy readItems(q, &GetItemsQuery::itemsAvailable);
        q->setParent(todoList.uid());
        cache.run(q);
        QVERIFY(readItems.wait());
        auto todo = Item::decache(readItems.at(0).at(0).toList().at(0), this);
        QCOMPARE(todo->property("percentageDone").toInt(), 50);
        QCOMPARE(todo->property("numSubtasks").toInt(), 2);
        QCOMPARE(todo->property("numDoneSubtasks").toInt(), 1);
    }

    task2.setDone(true);

    {
        auto q = new InsertOrUpdateItemsQuery;
        QSignalSpy finished(q, &InsertOrUpdateItemsQuery::finished);
        q->add(&task2);
        cache.run(q);
        QVERIFY(finished.wait());
    }

    {
        auto q = new GetItemsQuery;
        QSignalSpy readItems(q, &GetItemsQuery::itemsAvailable);
        q->setParent(todoList.uid());
        cache.run(q);
        QVERIFY(readItems.wait());
        auto todo = Item::decache(readItems.at(0).at(0).toList().at(0), this);
        QCOMPARE(todo->property("percentageDone").toInt(), 100);
        QCOMPARE(todo->property("numSubtasks").toInt(), 2);
        QCOMPARE(todo->property("numDoneSubtasks").toInt(), 2);
    }
}

QTEST_MAIN(GetItemsQueryTest)
#include "test_getitemsquery.moc"

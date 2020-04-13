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
    void cleanup() {}
    void cleanupTestCase() {}
};

void GetItemsQueryTest::run()
{
    //    QTest::ignoreMessage(QtDebugMsg,
    //                         QRegularExpression(".*Cache is uninitialized.*"));
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
                items.at(0).value<ItemCacheEntry>().toByteArray(),
                items.at(1).value<ItemCacheEntry>().toByteArray(),
                items.at(2).value<ItemCacheEntry>().toByteArray(),
                items.at(3).value<ItemCacheEntry>().toByteArray(),
                items.at(4).value<ItemCacheEntry>().toByteArray(),
        });
        QSet<QByteArray> expected = QSet<QByteArray>({
                todoList.encache().toByteArray(),
                todo.encache().toByteArray(),
                task.encache().toByteArray(),
                note.encache().toByteArray(),
                image.encache().toByteArray(),
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
        QSet<QByteArray> got =
                QSet<QByteArray>({ items.at(0).value<ItemCacheEntry>().toByteArray(),
                                   items.at(1).value<ItemCacheEntry>().toByteArray(),
                                   items.at(2).value<ItemCacheEntry>().toByteArray() });
        QSet<QByteArray> expected = QSet<QByteArray>({
                todoList.encache().toByteArray(),
                note.encache().toByteArray(),
                image.encache().toByteArray(),
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
        QSet<QByteArray> got =
                QSet<QByteArray>({ items.at(0).value<ItemCacheEntry>().toByteArray() });
        QSet<QByteArray> expected = QSet<QByteArray>({ todo.encache().toByteArray() });
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
        QSet<QByteArray> got =
                QSet<QByteArray>({ items.at(0).value<ItemCacheEntry>().toByteArray(),
                                   items.at(1).value<ItemCacheEntry>().toByteArray() });
        QSet<QByteArray> expected =
                QSet<QByteArray>({ todo.encache().toByteArray(), task.encache().toByteArray() });
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
        QSet<QByteArray> got =
                QSet<QByteArray>({ items.at(0).value<ItemCacheEntry>().toByteArray() });
        QSet<QByteArray> expected = QSet<QByteArray>({ task.encache().toByteArray() });
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
                items.at(0).value<ItemCacheEntry>().toByteArray(),
                items.at(1).value<ItemCacheEntry>().toByteArray(),
                items.at(2).value<ItemCacheEntry>().toByteArray(),
                items.at(3).value<ItemCacheEntry>().toByteArray(),
                items.at(4).value<ItemCacheEntry>().toByteArray(),
        });
        QSet<QByteArray> expected = QSet<QByteArray>({
                todoList.encache().toByteArray(),
                todo.encache().toByteArray(),
                task.encache().toByteArray(),
                note.encache().toByteArray(),
                image.encache().toByteArray(),
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
        QSet<QByteArray> got =
                QSet<QByteArray>({ items.at(0).value<ItemCacheEntry>().toByteArray(),
                                   items.at(1).value<ItemCacheEntry>().toByteArray(),
                                   items.at(2).value<ItemCacheEntry>().toByteArray() });
        QSet<QByteArray> expected = QSet<QByteArray>({
                todoList.encache().toByteArray(),
                note.encache().toByteArray(),
                image.encache().toByteArray(),
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
        QSet<QByteArray> got =
                QSet<QByteArray>({ items.at(0).value<ItemCacheEntry>().toByteArray() });
        QSet<QByteArray> expected = QSet<QByteArray>({ todo.encache().toByteArray() });
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
        QSet<QByteArray> got =
                QSet<QByteArray>({ items.at(0).value<ItemCacheEntry>().toByteArray() });
        QSet<QByteArray> expected = QSet<QByteArray>({ task.encache().toByteArray() });
        QCOMPARE(got, expected);
    }

    {
        auto q = new GetItemsQuery();
        q->setParent(todoList.uid());
        q->setItemFilter([=](ItemPtr item, GetItemsQuery *query) {
            for (auto i : query->childrenOf(item->uid())) {
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
        QSet<QByteArray> got =
                QSet<QByteArray>({ items.at(0).value<ItemCacheEntry>().toByteArray() });
        QSet<QByteArray> expected = QSet<QByteArray>({ todo.encache().toByteArray() });
        QCOMPARE(got, expected);
    }

    {
        auto q = new GetItemsQuery();
        q->setParent(todoList.uid());
        q->setItemFilter([=](ItemPtr, GetItemsQuery *) { return false; });
        QSignalSpy itemsAvailable(q, &GetItemsQuery::itemsAvailable);
        QSignalSpy destroyed(q, &GetItemsQuery::destroyed);
        cache.run(q);
        QVERIFY(destroyed.wait());
        QCOMPARE(itemsAvailable.count(), 1);
        auto items = itemsAvailable.at(0).at(0).toList();
        QCOMPARE(items.count(), 0);
    }
}

QTEST_MAIN(GetItemsQueryTest)
#include "test_getitemsquery.moc"

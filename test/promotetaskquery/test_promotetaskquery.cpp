/*
 * Copyright 2021 Martin Hoeher <martin@rpdev.net>
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
#include <QTest>

#include "datamodel/library.h"
#include "datamodel/task.h"
#include "datamodel/todo.h"
#include "datastorage/cache.h"
#include "datastorage/getitemsquery.h"
#include "datastorage/insertorupdateitemsquery.h"
#include "datastorage/promotetaskquery.h"

class PromoteTaskQueryTest : public QObject
{
    Q_OBJECT

private slots:

    void initTestCase() {}
    void init() {}
    void run();
    void cleanup() {}
    void cleanupTestCase() {}
};

void PromoteTaskQueryTest::run()
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
    Todo todo;
    todo.setTitle("A todo");
    todo.setTodoListUid(todoList.uid());
    Task task1;
    task1.setTitle("Task 1");
    task1.setTodoUid(todo.uid());
    Task task2;
    task2.setTitle("Task 2");
    task2.setTodoUid(todo.uid());
    Task task3;
    task3.setTitle("Task 3");
    task3.setTodoUid(todo.uid());
    TodoList todoList2;
    todoList2.setTitle("Another todo list");
    todoList2.setLibraryId(lib.uid());

    {
        auto q = new InsertOrUpdateItemsQuery;
        QSignalSpy finished(q, &InsertOrUpdateItemsQuery::finished);
        QSignalSpy destroyed(q, &InsertOrUpdateItemsQuery::destroyed);

        q->add(&lib);
        q->add(&todoList);
        q->add(&todoList2);
        q->add(&todo);
        q->add(&task1);
        q->add(&task2);
        q->add(&task3);
        cache.run(q);

        QVERIFY(destroyed.wait());
        QCOMPARE(finished.count(), 1);
    }

    {
        auto q = new GetItemsQuery;
        QSignalSpy itemsAvailable(q, &GetItemsQuery::itemsAvailable);
        QSignalSpy destroyed(q, &GetItemsQuery::destroyed);
        q->setParent(todo.uid());
        cache.run(q);
        QVERIFY(destroyed.wait());
        auto items = itemsAvailable.at(0).at(0).toList();
        QCOMPARE(items.length(), 3);
    }

    {
        auto q = new PromoteTaskQuery;
        QSignalSpy finished(q, &PromoteTaskQuery::finished);

        q->promoteTask(&task1, &todoList);
        cache.run(q);

        QVERIFY(finished.wait());
    }

    {
        auto q = new PromoteTaskQuery;
        QSignalSpy finished(q, &PromoteTaskQuery::finished);

        q->promoteTask(&task2, &todoList2);
        cache.run(q);

        QVERIFY(finished.wait());
    }

    {
        auto q = new GetItemsQuery;
        QSignalSpy itemsAvailable(q, &GetItemsQuery::itemsAvailable);
        QSignalSpy destroyed(q, &GetItemsQuery::destroyed);
        q->setParent(todoList.uid());
        cache.run(q);
        QVERIFY(destroyed.wait());
        auto items = itemsAvailable.at(0).at(0).toList();
        QCOMPARE(items.length(), 2);
        QSharedPointer<Item> loadedItem1(Item::decache(items.at(0)));
        QSharedPointer<Item> loadedItem2(Item::decache(items.at(1)));
        QVERIFY(loadedItem1->title() == task1.title() || loadedItem1->title() == todo.title());
        QVERIFY(loadedItem2->title() == task1.title() || loadedItem2->title() == todo.title());
        QVERIFY(loadedItem1->title() != loadedItem2->title());
    }

    {
        auto q = new GetItemsQuery;
        QSignalSpy itemsAvailable(q, &GetItemsQuery::itemsAvailable);
        QSignalSpy destroyed(q, &GetItemsQuery::destroyed);
        q->setParent(todoList2.uid());
        cache.run(q);
        QVERIFY(destroyed.wait());
        auto items = itemsAvailable.at(0).at(0).toList();
        QCOMPARE(items.length(), 1);
        QSharedPointer<Item> loadedItem(Item::decache(items.at(0)));
        QCOMPARE(loadedItem->title(), task2.title());
        QCOMPARE(loadedItem->uid(), task2.uid());
    }

    {
        auto q = new GetItemsQuery;
        QSignalSpy itemsAvailable(q, &GetItemsQuery::itemsAvailable);
        QSignalSpy destroyed(q, &GetItemsQuery::destroyed);
        q->setParent(todo.uid());
        cache.run(q);
        QVERIFY(destroyed.wait());
        auto items = itemsAvailable.at(0).at(0).toList();
        QCOMPARE(items.length(), 1);
        QSharedPointer<Item> loadedItem(Item::decache(items.at(0)));
        QCOMPARE(loadedItem->title(), task3.title());
        QCOMPARE(loadedItem->uid(), task3.uid());
    }
}

QTEST_MAIN(PromoteTaskQueryTest)
#include "test_promotetaskquery.moc"

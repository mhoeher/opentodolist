/*
 * Copyright 2020-2021 Martin Hoeher <martin@rpdev.net>
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
#include "datamodel/todo.h"
#include "datamodel/todolist.h"
#include "datastorage/cache.h"
#include "datastorage/getitemquery.h"
#include "datastorage/getitemsquery.h"
#include "datastorage/insertorupdateitemsquery.h"
#include "datastorage/movetodoquery.h"

class MoveTodoQueryTest : public QObject
{
    Q_OBJECT

private slots:

    void initTestCase() {}
    void init() {}
    void run();
    void cleanup() {}
    void cleanupTestCase() {}
};

void MoveTodoQueryTest::run()
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
        cache.run(q);

        QVERIFY(destroyed.wait());
        QCOMPARE(finished.count(), 1);
    }

    {
        auto q = new GetItemQuery();
        q->setUid(todo.uid());
        QSignalSpy itemLoaded(q, &GetItemQuery::itemLoaded);
        cache.run(q);
        QVERIFY(itemLoaded.wait());
        ItemPtr item(Item::decache(itemLoaded.value(0).value(0)));
        QVERIFY(item != nullptr);
        QCOMPARE(item->uid(), todo.uid());
        QCOMPARE(item->parentId(), todoList.uid());
    }

    {
        auto q = new MoveTodoQuery;
        QSignalSpy finished(q, &InsertOrUpdateItemsQuery::finished);

        q->moveTodo(&todo, &todoList2);
        cache.run(q);

        QVERIFY(finished.wait());
    }

    {
        auto q = new GetItemQuery();
        q->setUid(todo.uid());
        QSignalSpy itemLoaded(q, &GetItemQuery::itemLoaded);
        cache.run(q);
        QVERIFY(itemLoaded.wait());
        ItemPtr item(Item::decache(itemLoaded.value(0).value(0)));
        QVERIFY(item != nullptr);
        QCOMPARE(item->uid(), todo.uid());
        QCOMPARE(item->parentId(), todoList2.uid());
    }

    {
        auto q = new GetItemsQuery;
        QSignalSpy itemsAvailable(q, &GetItemsQuery::itemsAvailable);
        QSignalSpy destroyed(q, &GetItemsQuery::destroyed);
        q->setParent(todoList.uid());
        cache.run(q);
        QVERIFY(destroyed.wait());
        auto items = itemsAvailable.at(0).at(0).toList();
        QCOMPARE(items.length(), 0);
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
    }
}

QTEST_MAIN(MoveTodoQueryTest)
#include "test_movetodoquery.moc"

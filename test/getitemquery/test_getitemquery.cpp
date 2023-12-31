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
#include "datastorage/getitemquery.h"
#include "datastorage/insertorupdateitemsquery.h"

class GetItemQueryTest : public QObject
{
    Q_OBJECT

private slots:

    void initTestCase() {}
    void init() {}
    void run();
    void todolist_earliestChildDueDate();
    void cleanup() {}
    void cleanupTestCase() {}
};

void GetItemQueryTest::run()
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
        auto q = new GetItemQuery();
        q->setUid(todoList.uid());
        QSignalSpy itemLoaded(q, &GetItemQuery::itemLoaded);
        cache.run(q);
        QVERIFY(itemLoaded.wait());
        ItemPtr item(Item::decache(itemLoaded.value(0).value(0)));
        QVERIFY(item != nullptr);
        QCOMPARE(item->uid(), todoList.uid());
        QCOMPARE(item->title(), todoList.title());
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
        QCOMPARE(item->title(), todo.title());
    }

    {
        auto q = new GetItemQuery();
        q->setUid(task.uid());
        QSignalSpy itemLoaded(q, &GetItemQuery::itemLoaded);
        cache.run(q);
        QVERIFY(itemLoaded.wait());
        auto cacheEntry = itemLoaded.value(0).value(0).value<ItemCacheEntry>();
        ItemPtr item(Item::decache(cacheEntry));
        QVERIFY(item != nullptr);
        QCOMPARE(item->uid(), task.uid());
        QCOMPARE(item->title(), task.title());

        // Check if recursive extraction of all parent items work (we pick task as it is furthest
        // down the chain, but as the implementation is generic, it should work for any other item
        // as well).
        cacheEntry = itemLoaded.value(0).value(1).toList().value(0).value<ItemCacheEntry>();
        ItemPtr todoItem(Item::decache(cacheEntry));
        QVERIFY(todoItem != nullptr);
        QCOMPARE(todoItem->uid(), todo.uid());
        QCOMPARE(todoItem->title(), todo.title());

        cacheEntry = itemLoaded.value(0).value(1).toList().value(1).value<ItemCacheEntry>();
        ItemPtr todoListItem(Item::decache(cacheEntry));
        QVERIFY(todoListItem != nullptr);
        QCOMPARE(todoListItem->uid(), todoList.uid());
        QCOMPARE(todoListItem->title(), todoList.title());

        auto libraryCacheEntry = itemLoaded.value(0).value(2).value<LibraryCacheEntry>();
        QSharedPointer<Library> libPtr(Library::decache(libraryCacheEntry));
        QVERIFY(libPtr != nullptr);
        QCOMPARE(libPtr->uid(), lib.uid());
        QCOMPARE(libPtr->name(), lib.name());
    }

    {
        auto q = new GetItemQuery();
        q->setUid(note.uid());
        QSignalSpy itemLoaded(q, &GetItemQuery::itemLoaded);
        cache.run(q);
        QVERIFY(itemLoaded.wait());
        ItemPtr item(Item::decache(itemLoaded.value(0).value(0)));
        QVERIFY(item != nullptr);
        QCOMPARE(item->uid(), note.uid());
        QCOMPARE(item->title(), note.title());
    }

    {
        auto q = new GetItemQuery();
        q->setUid(image.uid());
        QSignalSpy itemLoaded(q, &GetItemQuery::itemLoaded);
        cache.run(q);
        QVERIFY(itemLoaded.wait());
        ItemPtr item(Item::decache(itemLoaded.value(0).value(0)));
        QVERIFY(item != nullptr);
        QCOMPARE(item->uid(), image.uid());
        QCOMPARE(item->title(), image.title());
    }
}

void GetItemQueryTest::todolist_earliestChildDueDate()
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
    todo1.setTitle("A todo");
    todo1.setTodoListUid(todoList.uid());
    Todo todo2;
    todo2.setTitle("Another todo");
    todo2.setTodoListUid(todoList.uid());

    {
        auto q = new InsertOrUpdateItemsQuery;
        QSignalSpy finished(q, &InsertOrUpdateItemsQuery::finished);
        QSignalSpy destroyed(q, &InsertOrUpdateItemsQuery::destroyed);

        q->add(&lib);
        q->add(&todoList);
        q->add(&todo1);
        q->add(&todo2);
        cache.run(q);

        QVERIFY(destroyed.wait());
        QCOMPARE(finished.count(), 1);
    }

    {
        auto q = new GetItemQuery();
        q->setUid(todoList.uid());
        QSignalSpy itemLoaded(q, &GetItemQuery::itemLoaded);
        cache.run(q);
        QVERIFY(itemLoaded.wait());
        ItemPtr item(Item::decache(itemLoaded.value(0).value(0)));
        QVERIFY(item != nullptr);
        QCOMPARE(item->uid(), todoList.uid());
        QCOMPARE(item->title(), todoList.title());
        TodoListPtr todoListItem = qSharedPointerCast<TodoList>(item);
        QVERIFY(todoListItem->dueTo().isNull());
    }

    todo2.setDueTo(QDateTime::currentDateTime());
    todo1.setDueTo(todo2.dueTo().addDays(1));

    {
        auto q = new InsertOrUpdateItemsQuery;
        QSignalSpy finished(q, &InsertOrUpdateItemsQuery::finished);
        QSignalSpy destroyed(q, &InsertOrUpdateItemsQuery::destroyed);

        q->add(&todo1);
        q->add(&todo2);
        cache.run(q);

        QVERIFY(destroyed.wait());
        QCOMPARE(finished.count(), 1);
    }

    {
        auto q = new GetItemQuery();
        q->setUid(todoList.uid());
        QSignalSpy itemLoaded(q, &GetItemQuery::itemLoaded);
        cache.run(q);
        QVERIFY(itemLoaded.wait());
        ItemPtr item(Item::decache(itemLoaded.value(0).value(0)));
        QVERIFY(item != nullptr);
        QCOMPARE(item->uid(), todoList.uid());
        QCOMPARE(item->title(), todoList.title());
        TodoListPtr todoListItem = qSharedPointerCast<TodoList>(item);
        QCOMPARE(todoListItem->earliestChildDueTo(), todo2.dueTo());
        QCOMPARE(todoListItem->effectiveDueTo(), todo2.dueTo());
    }

    todo2.setDone(true);

    {
        auto q = new InsertOrUpdateItemsQuery;
        QSignalSpy finished(q, &InsertOrUpdateItemsQuery::finished);
        QSignalSpy destroyed(q, &InsertOrUpdateItemsQuery::destroyed);

        q->add(&todo2);
        cache.run(q);

        QVERIFY(destroyed.wait());
        QCOMPARE(finished.count(), 1);
    }

    {
        auto q = new GetItemQuery();
        q->setUid(todoList.uid());
        QSignalSpy itemLoaded(q, &GetItemQuery::itemLoaded);
        cache.run(q);
        QVERIFY(itemLoaded.wait());
        ItemPtr item(Item::decache(itemLoaded.value(0).value(0)));
        QVERIFY(item != nullptr);
        QCOMPARE(item->uid(), todoList.uid());
        QCOMPARE(item->title(), todoList.title());
        TodoListPtr todoListItem = qSharedPointerCast<TodoList>(item);
        QCOMPARE(todoListItem->earliestChildDueTo(), todo1.dueTo());
        QCOMPARE(todoListItem->effectiveDueTo(), todo1.dueTo());
    }

    todoList.setDueTo(todo1.dueTo().addDays(1));
    auto expectedDueTo = todoList.dueTo();

    {
        auto q = new InsertOrUpdateItemsQuery;
        QSignalSpy finished(q, &InsertOrUpdateItemsQuery::finished);
        QSignalSpy destroyed(q, &InsertOrUpdateItemsQuery::destroyed);

        q->add(&todoList);
        cache.run(q);

        QVERIFY(destroyed.wait());
        QCOMPARE(finished.count(), 1);
    }

    {
        auto q = new GetItemQuery();
        q->setUid(todoList.uid());
        QSignalSpy itemLoaded(q, &GetItemQuery::itemLoaded);
        cache.run(q);
        QVERIFY(itemLoaded.wait());
        ItemPtr item(Item::decache(itemLoaded.value(0).value(0)));
        QVERIFY(item != nullptr);
        QCOMPARE(item->uid(), todoList.uid());
        QCOMPARE(item->title(), todoList.title());
        TodoListPtr todoListItem = qSharedPointerCast<TodoList>(item);
        QCOMPARE(todoListItem->earliestChildDueTo(), todo1.dueTo());
        QCOMPARE(todoListItem->effectiveDueTo(), expectedDueTo);
    }
}

QTEST_MAIN(GetItemQueryTest)
#include "test_getitemquery.moc"

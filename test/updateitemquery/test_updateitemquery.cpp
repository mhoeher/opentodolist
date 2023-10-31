/*
 * Copyright 2023 Martin Hoeher <martin@rpdev.net>
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
#include <QTemporaryDir>
#include <QTest>

#include "datastorage/updateitemquery.h"
#include "datamodel/library.h"
#include "datamodel/task.h"
#include "datamodel/todo.h"
#include "datamodel/todolist.h"
#include "datastorage/cache.h"
#include "datastorage/insertorupdateitemsquery.h"

#include "../test-utils.h"

static const char* UpdateItemScript = R"(
function updateItem(item) {
    if (item.title === "Another todo") {
        item.title = "Changed todo";
    } else if (item.title === "Another task") {
        item.title = "Changed task";
    }
}
)";
class UpdateItemQueryTest : public QObject
{
    Q_OBJECT

private slots:

    void initTestCase() {}
    void init() {}
    void testUpdateItems();
    void cleanup() {}
    void cleanupTestCase() {}
};

void UpdateItemQueryTest::testUpdateItems()
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
    Todo todo2;
    todo2.setTitle("Another todo");
    todo2.setTodoListUid(todoList.uid());
    Task task;
    task.setTitle("A task");
    task.setTodoUid(todo.uid());
    Task task2;
    task2.setTitle("Another task");
    task2.setTodoUid(todo2.uid());

    {
        auto q = new InsertOrUpdateItemsQuery;
        QSignalSpy finished(q, &InsertOrUpdateItemsQuery::finished);

        q->add(&lib);
        q->add(&todoList);
        q->add(&todo);
        q->add(&todo2);
        q->add(&task);
        q->add(&task2);
        cache.run(q);

        QVERIFY(finished.wait());
    }

    // Update the second task/todo items
    {
        auto q = new UpdateItemQuery;
        QSignalSpy finished(q, &UpdateItemQuery::finished);

        q->setUid(todoList.uid());
        q->setScript(UpdateItemScript);
        cache.run(q);

        QVERIFY(finished.wait());
    }

    // Check if the items are as we expect them:
    {
        auto storedTodoList = getItemFromCacheByUid<TodoList>(&cache, todoList.uid());
        QVERIFY(storedTodoList);
        QCOMPARE(todoList.encache().toByteArray(), storedTodoList->encache().toByteArray());
    }
    {
        auto storedTodo = getItemFromCacheByUid<Todo>(&cache, todo.uid());
        QVERIFY(storedTodo);
        QCOMPARE(todo.encache().toByteArray(), storedTodo->encache().toByteArray());
    }
    {
        auto storedTodo2 = getItemFromCacheByUid<Todo>(&cache, todo2.uid());
        QVERIFY(storedTodo2);
        QVERIFY(todo2.encache().toByteArray() != storedTodo2->encache().toByteArray());
        QCOMPARE(storedTodo2->title(), "Changed todo");
    }
    {
        auto storedTask = getItemFromCacheByUid<Task>(&cache, task.uid());
        QVERIFY(storedTask);
        QCOMPARE(task.encache().toByteArray(), storedTask->encache().toByteArray());
    }
    {
        auto storedTask2 = getItemFromCacheByUid<Task>(&cache, task2.uid());
        QVERIFY(storedTask2);
        QVERIFY(task2.encache().toByteArray() != storedTask2->encache().toByteArray());
        QCOMPARE(storedTask2->title(), "Changed task");
    }
}

QTEST_MAIN(UpdateItemQueryTest)
#include "test_updateitemquery.moc"

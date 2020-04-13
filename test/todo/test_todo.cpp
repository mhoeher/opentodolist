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
#include <QSignalSpy>
#include <QTemporaryDir>
#include <QTest>

#include "datamodel/todo.h"
#include "datamodel/task.h"

class TodoTest : public QObject
{
    Q_OBJECT

private slots:

    void initTestCase() {}
    void testProperties();
    void testPersistence();
    void cleanupTestCase() {}
};

void TodoTest::testProperties()
{
    Todo todo;
    QSignalSpy doneChanged(&todo, &Todo::doneChanged);
    QSignalSpy todoListUidChanged(&todo, &Todo::todoListUidChanged);

    QVERIFY(!todo.done());

    QUuid uid = QUuid::createUuid();

    todo.setDone(true);
    todo.setTodoListUid(uid);

    QCOMPARE(doneChanged.count(), 1);
    QCOMPARE(todoListUidChanged.count(), 1);

    QVERIFY(todo.done());
    QCOMPARE(todo.todoListUid(), uid);
}

void TodoTest::testPersistence()
{
    Todo todo, anotherTodo;
    QSignalSpy doneChanged(&anotherTodo, &Todo::doneChanged);
    QSignalSpy todoListUidChanged(&anotherTodo, &Todo::todoListUidChanged);

    QVERIFY(!todo.done());
    QCOMPARE(todo.progress(), -1);
    QCOMPARE(todo.percentageDone(), 0);

    QUuid uid = QUuid::createUuid();

    todo.setDone(true);
    todo.setTodoListUid(uid);
    todo.setProgress(50);

    QCOMPARE(todo.progress(), 50);
    QCOMPARE(todo.percentageDone(), 50);

    anotherTodo.fromVariant(todo.toVariant());

    QCOMPARE(doneChanged.count(), 1);
    QCOMPARE(todoListUidChanged.count(), 1);

    QVERIFY(anotherTodo.done());
    QCOMPARE(anotherTodo.todoListUid(), uid);
    QCOMPARE(anotherTodo.progress(), 50);
    QCOMPARE(anotherTodo.percentageDone(), 50);
}

QTEST_MAIN(TodoTest)
#include "test_todo.moc"

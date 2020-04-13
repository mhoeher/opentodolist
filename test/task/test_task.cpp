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

#include "datamodel/task.h"

class TaskTest : public QObject
{
    Q_OBJECT

private slots:

    void initTestCase() {}
    void testProperties();
    void testPersistence();
    void cleanupTestCase() {}
};

void TaskTest::testProperties()
{
    Task task;
    QSignalSpy doneChanged(&task, &Task::doneChanged);
    QSignalSpy todoUidChanged(&task, &Task::todoUidChanged);

    QVERIFY(!task.done());

    QUuid uuid = QUuid::createUuid();

    task.setDone(true);
    task.setTodoUid(uuid);

    QCOMPARE(doneChanged.count(), 1);
    QCOMPARE(todoUidChanged.count(), 1);

    QVERIFY(task.done());
    QCOMPARE(task.todoUid(), uuid);
}

void TaskTest::testPersistence()
{
    Task task, anotherTask;
    QSignalSpy doneChanged(&anotherTask, &Task::doneChanged);
    QSignalSpy todoUidChanged(&anotherTask, &Task::todoUidChanged);

    QUuid uuid = QUuid::createUuid();

    task.setDone(true);
    task.setTodoUid(uuid);

    anotherTask.fromVariant(task.toVariant());

    QCOMPARE(doneChanged.count(), 1);
    QCOMPARE(todoUidChanged.count(), 1);

    QVERIFY(anotherTask.done());
    QCOMPARE(anotherTask.todoUid(), uuid);
}

QTEST_MAIN(TaskTest)
#include "test_task.moc"

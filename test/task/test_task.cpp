#include "datamodel/task.h"

#include <QObject>
#include <QSignalSpy>
#include <QTemporaryDir>
#include <QTest>

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

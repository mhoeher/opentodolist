#include "datamodel/todo.h"
#include "datamodel/task.h"

#include <QObject>
#include <QSignalSpy>
#include <QTemporaryDir>
#include <QTest>


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

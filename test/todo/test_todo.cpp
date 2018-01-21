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

    QUuid uid = QUuid::createUuid();

    todo.setDone(true);
    todo.setTodoListUid(uid);

    anotherTodo.fromVariant(todo.toVariant());

    QCOMPARE(doneChanged.count(), 1);
    QCOMPARE(todoListUidChanged.count(), 1);

    QVERIFY(anotherTodo.done());
    QCOMPARE(anotherTodo.todoListUid(), uid);
}


QTEST_MAIN(TodoTest)
#include "test_todo.moc"

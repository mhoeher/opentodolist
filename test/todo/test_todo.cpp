#include "todo.h"
#include "task.h"

#include <QObject>
#include <QTemporaryDir>
#include <QTest>


class TodoTest : public QObject 
{
  Q_OBJECT
  
private slots:
  
  void initTestCase() {}
  void testConstructor();
  void testPersistence();
  void testAddTask();
  void testTasks();
  void testDeleteTask();
  void cleanupTestCase() {}

};


void TodoTest::testConstructor()
{
  QTemporaryDir dir;
  QVERIFY2(dir.isValid(), "Failed to create temporary directory.");
  auto todo = new Todo(dir.path());
  Q_CHECK_PTR(todo);
  delete todo;
}

void TodoTest::testPersistence()
{
  QTemporaryDir dir;
  QVERIFY2(dir.isValid(), "Failed to create temporary directory.");
  auto todo = new Todo(dir.path());
  Q_CHECK_PTR(todo);
  QVERIFY2(!todo->done(), "Expected todo to be not done.");
  todo->setDone(true);
  todo->commitItem();
  delete todo;
  todo = new Todo(dir.path());
  Q_CHECK_PTR(todo);
  QVERIFY2(todo->done(), "Expected todo to be done.");
  delete todo;
}

void TodoTest::testAddTask()
{
  QTemporaryDir dir;
  QVERIFY2(dir.isValid(), "Failed to create temporary directory.");
  auto todo = new Todo(dir.path());
  Q_CHECK_PTR(todo);
  auto task = todo->addTask("Some Task");
  Q_CHECK_PTR(task);
  QCOMPARE(task->todo(), todo);
  delete todo;
}

void TodoTest::testTasks()
{
  QTemporaryDir dir;
  QVERIFY2(dir.isValid(), "Failed to create temporary directory.");
  auto todo = new Todo(dir.path());
  Q_CHECK_PTR(todo);
  auto task = todo->addTask("Some Task");
  Q_CHECK_PTR(task);
  delete todo;
  todo = new Todo(dir.path());
  TaskList tasks = todo->tasks();
  QCOMPARE(tasks.size(), 1);
  Q_CHECK_PTR(tasks.at(0));
  QCOMPARE(tasks.at(0)->title(), QString("Some Task"));
  delete todo;
}

void TodoTest::testDeleteTask()
{
  QTemporaryDir dir;
  QVERIFY2(dir.isValid(), "Failed to create temporary directory.");
  auto todo = new Todo(dir.path());
  Q_CHECK_PTR(todo);
  auto task = todo->addTask("Some Task");
  Q_CHECK_PTR(task);
  TaskList tasks = todo->tasks();
  QCOMPARE(tasks.size(), 1);
  QVERIFY2(task->deleteItem(), "Failed to delete task.");
  tasks = todo->tasks();
  QCOMPARE(tasks.size(), 0);
  delete todo;
}


QTEST_MAIN(TodoTest)
#include "test_todo.moc"

#include "todolist.h"
#include "todo.h"

#include <QObject>
#include <QTemporaryDir>
#include <QTest>


class TodoListTest : public QObject 
{
  Q_OBJECT
  
private slots:
  
  void initTestCase() {}
  void testConstructor();
  void testAddTodo();
  void testTodos();
  void testDeleteTodo();
  void cleanupTestCase() {}
  
};


void TodoListTest::testConstructor()
{
  QTemporaryDir dir;
  QVERIFY2(dir.isValid(), "Failed to create temporary directory.");
  auto todoList = new TodoList(dir.path());
  Q_CHECK_PTR(todoList);
  delete todoList;  
}

void TodoListTest::testAddTodo()
{
  QTemporaryDir dir;
  QVERIFY2(dir.isValid(), "Failed to create temporary directory.");
  auto todoList = new TodoList(dir.path());
  Q_CHECK_PTR(todoList);
  auto todo = todoList->addTodo("Some Todo");
  Q_CHECK_PTR(todo);
  QCOMPARE(todo->title(), QString("Some Todo"));
  delete todoList;
}

void TodoListTest::testTodos()
{
  QTemporaryDir dir;
  QVERIFY2(dir.isValid(), "Failed to create temporary directory.");
  auto todoList = new TodoList(dir.path());
  Q_CHECK_PTR(todoList);
  auto todo = todoList->addTodo("Some Todo");
  Q_CHECK_PTR(todo);
  QCOMPARE(todoList->todos().size(), 1);
  delete todoList;
  todoList = new TodoList(dir.path());
  Q_CHECK_PTR(todoList);
  TodosList todos = todoList->todos();
  QCOMPARE(todos.size(), 1);
  Q_CHECK_PTR(todos.at(0));
  QCOMPARE(todos.at(0)->title(), QString("Some Todo"));
  delete todoList;
}

void TodoListTest::testDeleteTodo()
{
  QTemporaryDir dir;
  QVERIFY2(dir.isValid(), "Failed to create temporary directory.");
  auto todoList = new TodoList(dir.path());
  Q_CHECK_PTR(todoList);
  auto todo = todoList->addTodo("Some Todo");
  Q_CHECK_PTR(todo);
  QCOMPARE(todoList->todos().size(), 1);
  todo->deleteItem();
  QCOMPARE(todoList->todos().size(), 0);
  delete todoList;
}

QTEST_MAIN(TodoListTest)
#include "test_todolist.moc"

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
  void cleanupTestCase() {}

};

QTEST_MAIN(TodoListTest)
#include "test_todolist.moc"

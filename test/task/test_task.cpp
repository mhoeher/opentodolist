#include "task.h"

#include <QObject>
#include <QTemporaryDir>
#include <QTest>

class TaskTest : public QObject
{
  Q_OBJECT

private slots:

  void initTestCase() {}
  void testConstructor();
  void testPersistence();
  void cleanupTestCase() {}
  
};

void TaskTest::testConstructor()
{
  QTemporaryDir dir;
  QVERIFY2(dir.isValid(), "Failed to create temporary directory.");
  auto task = new Task(dir.path());
  Q_CHECK_PTR(task);
  delete task;
}

void TaskTest::testPersistence()
{
  QTemporaryDir dir;
  QVERIFY2(dir.isValid(), "Failed to create temporary directory.");
  auto task = new Task(dir.path());
  Q_CHECK_PTR(task);
  QVERIFY2(!task->done(), "Expected task not to be done.");
  task->setDone(true);
  task->commitItem();
  delete task;
  task = new Task(dir.path());
  QVERIFY2(task->done(), "Expected task to be done.");
  delete task;
}


QTEST_MAIN(TaskTest)
#include "test_task.moc"

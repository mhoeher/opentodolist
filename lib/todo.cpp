#include "todo.h"

#include "task.h"
#include "todolist.h"

#include <QDir>

/**
   @brief The item type name used by the Todo class.
 */
const QString Todo::ItemType = "Todo";

/**
  @brief The list of persistent properties of the Todo class.
  */
const QStringList Todo::PersistentProperties = {"done"};

/**
   @brief Constructor.
 */
Todo::Todo(const QString &directory, QObject *parent) :
  ComplexItem(false, directory, ItemType, PersistentProperties, parent),
  m_done(false),
  m_tasks(),
  m_tasksLoaded(false)
{
  initializeItem();
}

/**
   @brief Set the value of the done property.
 */
void Todo::setDone(bool done)
{
  if (m_done != done) {
    m_done = done;
    emit doneChanged();
    saveItem();
  }
}

/**
   @brief The todo list to which the todo belongs.
 */
TodoList *Todo::todoList() const 
{
  return m_todoList.data(); 
}

/**
   @brief Adds a Task to the todo.
   
   This creates a new task with the given @p title and adds it
   to the todo. The created task is returned.
   
   @note The task returned is owned by the todo.
 */
Task *Todo::addTask(const QString &title)
{
  QString tasksDir(directory() + "/tasks/"); 
  QString tpl = titleToDirectoryName(title);
  QString taskDir = tasksDir + tpl;
  int i = 0;
  while (QDir(taskDir).exists()) {
    taskDir = tasksDir + tpl + " - " + QString::number(i++);
  }
  Task *task = new Task(taskDir, this);
  Q_CHECK_PTR(task);
  task->setTitle(title);
  task->saveItem(SaveItemImmediately);
  appendTask(task);
  return task;
}

/**
   @brief Returns the tasks belonging to the todo.
 */
TaskList Todo::tasks()
{
  loadTasks();
  return m_tasks;
}

QQmlListProperty<Task> Todo::taskList()
{
  return QQmlListProperty<Task>(this, nullptr, taskListCount, taskListAt);
}

void Todo::appendTask(Task *task)
{
  Q_CHECK_PTR(task);
  task->setTodo(this);
  connect(task, &Task::itemDeleted, this, &Todo::onTaskDeleted);
  m_tasks.append(task);
  emit tasksChanged();
}

void Todo::loadTasks()
{
  if (!m_tasksLoaded) {
    QString tasksDir = directory() + "/tasks/";
    QDir dir(tasksDir);
    for (auto entry : dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
      QString taskDir = dir.absoluteFilePath(entry);
      if (Item::isItemDirectory<Task>(taskDir)) {
        Task *task = new Task(taskDir, this);
        Q_CHECK_PTR(task);
        if (hasTask(task->uid())) {
          delete task;
          continue;
        }
        appendTask(task);
      }
    }
    m_tasksLoaded = true;
  }
}

bool Todo::hasTask(const QUuid &uuid)
{
  for (auto task : m_tasks) {
    if (task->uid() == uuid) {
      return true;
    }
  }
  return false;
}

/**
   @brief Sets the todo list pointer to which the todo belongs.
 */
void Todo::setTodoList(TodoList *todoList)
{
  m_todoList = todoList;
}

int Todo::taskListCount(QQmlListProperty<Task> *property)
{
  Todo *_this = dynamic_cast<Todo*>(property->object);
  Q_CHECK_PTR(_this);
  return _this->tasks().size();
}

Task *Todo::taskListAt(QQmlListProperty<Task> *property, int index)
{
  Todo *_this = dynamic_cast<Todo*>(property->object);
  Q_CHECK_PTR(_this);
  return _this->tasks().at(index);
}

void Todo::onTaskDeleted(Item *item)
{
  Q_CHECK_PTR(item);
  for (int i = 0; i < m_tasks.size(); ++i) {
    auto task = m_tasks.at(i);
    if (task->uid() == item->uid()) {
      m_tasks.removeAt(i);
      task->deleteLater();
      emit tasksChanged();
      break;
    }
  }
}


#include "todo.h"

#include "fileutils.h"
#include "task.h"
#include "todolist.h"

#include <QDir>
#include <QSet>

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

void Todo::handleFileChanged(const QString &filename)
{
    for (auto task : m_tasks) {
        if (FileUtils::isSubDirOrFile(task->directory(), filename))
        {
            task->handleFileChanged(filename);
            return;
        }
    }
    deleteDanglingTasks();
    scanTasks();
    ComplexItem::handleFileChanged(filename);
}

bool Todo::deleteItemData()
{
    bool result = true;
    for (int i = m_tasks.length() - 1; i >= 0; --i)
    {
        result = m_tasks[i]->deleteItem() && result;
    }
    QDir dir(directory());
    if (dir.exists() && dir.exists("tasks"))
    {
        result = dir.rmdir("tasks") && result;
    }
    result = ComplexItem::deleteItemData() && result;
    return result;
}

void Todo::appendTask(Task *task)
{
    Q_CHECK_PTR(task);
    task->setTodo(this);
    connect(task, &Task::itemDeleted, this, &Todo::onTaskDeleted);
    connect(task, &QObject::destroyed, this, &Todo::onItemDeleted);
    m_tasks.append(task);
    emit tasksChanged();
}

void Todo::loadTasks()
{
    if (!m_tasksLoaded) {
        m_tasksLoaded = true;
        scanTasks();
    }
}

void Todo::deleteDanglingTasks()
{
    for (auto task : m_tasks) {
        if (task->isDangling()) {
            task->deleteItem();
        }
    }
}

void Todo::scanTasks()
{
    QSet<QString> taskDirs;
    for (auto task : m_tasks) {
        taskDirs.insert(task->directory());
    }
    QDir tasksDir(directory());
    if (tasksDir.cd("tasks")) {
        for (auto entry : tasksDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
            QString taskDir = tasksDir.absoluteFilePath(entry);
            if (!taskDirs.contains(taskDir)) {
                if (Item::isItemDirectory<Task>(taskDir)) {
                    Task *task = new Task(taskDir, this);
                    Q_CHECK_PTR(task);
                    if (containsTask(task->uid()))
                    {
                        delete task;
                    }
                    else
                    {
                        appendTask(task);
                    }
                }
            }
        }
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

/**
   @brief Returns true of the todo contains a task with the given @p uuid.
 */
bool Todo::containsTask(const QUuid& uuid) const
{
    bool result = false;
    for (const auto &task : m_tasks)
    {
        if (task->uid() == uuid)
        {
            result = true;
            break;
        }
    }
    return result;
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
        if (task == item) {
            m_tasks.removeAt(i);
            task->deleteLater();
            emit tasksChanged();
            break;
        }
    }
}

void Todo::onItemDeleted(QObject* item)
{
    Q_CHECK_PTR(item);
    if (m_tasks.removeOne(reinterpret_cast<Task*>(item)))
    {
        emit tasksChanged();
    }
}


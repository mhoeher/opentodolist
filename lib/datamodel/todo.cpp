#include "todo.h"

#include <QQmlEngine>

#include "library.h"
#include "task.h"


/**
 * @brief Constructor.
 */
Todo::Todo(const QString &filename, QObject *parent) : ComplexItem(filename, parent),
    m_todoListUid(),
    m_done(false),
    m_library(nullptr)
{
    connect(this, &Todo::todoListUidChanged, this, &ComplexItem::changed);
    connect(this, &Todo::doneChanged, this, &ComplexItem::changed);
}

/**
 * @brief Constructor.
 */
Todo::Todo(QObject* parent) : Todo(QString(), parent)
{
}

/**
 * @brief Constructor.
 */
Todo::Todo(const QDir& dir, QObject* parent) : ComplexItem(dir, parent),
    m_todoListUid(),
    m_done(false),
    m_library(nullptr)
{
    connect(this, &Todo::todoListUidChanged, this, &ComplexItem::changed);
    connect(this, &Todo::doneChanged, this, &ComplexItem::changed);
}

/**
 * @brief Destructor.
 */
Todo::~Todo()
{

}

QUuid Todo::parentId() const
{
    return m_todoListUid;
}

/**
 * @brief A boolean flag indicating if the todo is already done.
 */
bool Todo::done() const
{
    return m_done;
}

/**
   @brief Set the value of the done property.
 */
void Todo::setDone(bool done)
{
    if (m_done != done) {
        m_done = done;
        emit doneChanged();
        save();
    }
}

/**
 * @brief The UUID of the todo list the todo belongs to.
 */
QUuid Todo::todoListUid() const
{
    return m_todoListUid;
}

/**
 * @brief Set the todo list UUID.
 */
void Todo::setTodoListUid(const QUuid& todoListUid)
{
    if (m_todoListUid != todoListUid) {
        m_todoListUid = todoListUid;
        emit todoListUidChanged();
        save();
    }
}

/**
 * @brief Create a new task.
 *
 * This creates a new task in the library and returns a pointer to it.
 * Note that this method is intended to be called by QML. It is safe to
 * use the returned object in JavaScript/QML. When invoked from C++, do not
 * attempt to store the returned object for access over a longer time,
 * as the library might delete it at any time if need be and hence the pointer
 * might get dangling.
 */
Task*Todo::addTask()
{
    if (m_library) {
        TaskPtr task;
        if (m_library->isValid()) {
            QDir dir(m_library->newItemLocation());
            dir.mkpath(".");
            task = TaskPtr(new Task(dir));
        } else {
            task = TaskPtr(new Task());
        }
        m_library->tasks()->addItem(task);
        task->setTodoUid(uid());
        task->setWeight(m_library->tasks()->nextItemWeight());
        QQmlEngine::setObjectOwnership(task.data(), QQmlEngine::CppOwnership);
        task->save();
        return task.data();
    }
    return nullptr;
}


/**
 * @brief Get the completion state of the todo.
 *
 * This returns the percentage (as a number between 0 and 100) the todo is
 * done. That value is derived from the number of tasks the todo contains and
 * how many of them are done.
 */
int Todo::percentageDone() const
{
    auto uid = this->uid();
    int totalTasks = 0;
    int completeTasks = 0;
    if (m_library != nullptr) {
        auto tasks = m_library->tasks();
        for (int i = 0; i < tasks->count(); ++i) {
            auto task = dynamic_cast<Task*>(tasks->get(i));
            if (task != nullptr) {
                if (task->todoUid() == uid) {
                    ++totalTasks;
                    if (task->done()) {
                        ++completeTasks;
                    }
                }
            }
        }
    }
    if (totalTasks > 0) {
        return completeTasks * 100 / totalTasks;
    } else {
        return 0;
    }
}


QVariantMap Todo::toMap() const
{
    auto result = ComplexItem::toMap();
    result["todoListUid"] = m_todoListUid;
    result["done"] = m_done;
    return result;
}

void Todo::fromMap(QVariantMap map)
{
    ComplexItem::fromMap(map);
    setTodoListUid(map.value("todoListUid", m_todoListUid).toUuid());
    setDone(map.value("done", m_done).toBool());
}


void Todo::setLibrary(Library *library)
{
    if (m_library != nullptr) {
        disconnect(m_library->tasks(), &ItemContainer::countChanged,
                   this, &Todo::percentageDoneChanged);
        disconnect(m_library->tasks(), &ItemContainer::itemChanged,
                   this, &Todo::handleTaskChanged);
    }
    m_library = library;
    if (m_library != nullptr) {
        connect(m_library->tasks(), &ItemContainer::countChanged,
                this, &Todo::percentageDoneChanged);
        connect(m_library->tasks(), &ItemContainer::itemChanged,
                this, &Todo::handleTaskChanged);
    }
    emit percentageDoneChanged();
}


void Todo::handleTaskChanged(int index)
{
    auto task = dynamic_cast<Task*>(m_library->tasks()->get(index));
    if (task->todoUid() == uid()) {
        emit percentageDoneChanged();
    }
}


Library *Todo::library() const
{
    return m_library;
}

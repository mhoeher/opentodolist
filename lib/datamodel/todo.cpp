#include "todo.h"

#include <QQmlEngine>

#include "library.h"
#include "task.h"


/**
 * @brief Constructor.
 */
Todo::Todo(const QString &filename, QObject *parent) :
    ComplexItem(filename, parent),
    m_todoListUid(),
    m_done(false)
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
    m_done(false)
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
    }
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
    // @todo Implement me
//    auto uid = this->uid();
//    int totalTasks = 0;
//    int completeTasks = 0;
//    if (m_library != nullptr) {
//        auto tasks = m_library->tasks();
//        for (int i = 0; i < tasks->count(); ++i) {
//            auto task = dynamic_cast<Task*>(tasks->get(i));
//            if (task != nullptr) {
//                if (task->todoUid() == uid) {
//                    ++totalTasks;
//                    if (task->done()) {
//                        ++completeTasks;
//                    }
//                }
//            }
//        }
//    }
//    if (totalTasks > 0) {
//        return completeTasks * 100 / totalTasks;
//    } else {
//        return 0;
//    }
    return 0;
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

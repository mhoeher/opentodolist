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
            task = TaskPtr(new Task(QDir(m_library->directory())));
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

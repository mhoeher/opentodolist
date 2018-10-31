#include "task.h"


Task::Task(QObject* parent) : Task(QString(), parent)
{
}

Task::Task(const QDir& dir, QObject* parent) : Item(dir, parent),
    m_done(false),
    m_todoUid()
{
    connect(this, &Task::doneChanged, this, &Task::changed);
    connect(this, &Task::todoUidChanged, this, &Task::changed);
}

Task::Task(const QString &filename, QObject* parent) : Item(filename, parent),
    m_done(false),
    m_todoUid(QUuid())
{
    connect(this, &Task::doneChanged, this, &Task::changed);
    connect(this, &Task::todoUidChanged, this, &Task::changed);
}

Task::~Task()
{
}

QUuid Task::parentId() const
{
    return m_todoUid;
}

/**
 * @brief A boolean value indicating if the task is done or not.
 */
bool Task::done() const
{
    return m_done;
}

/**
 * @brief Set the task's done property.
 */
void Task::setDone(bool done)
{
    if (m_done != done) {
        m_done = done;
        emit doneChanged();
    }
}

/**
 * @brief The UUID of the todo the task belongs to.
 */
QUuid Task::todoUid() const
{
    return m_todoUid;
}

void Task::setTodoUid(const QUuid& todoUid)
{
    if (m_todoUid != todoUid) {
        m_todoUid = todoUid;
        emit todoUidChanged();
    }
}

QVariantMap Task::toMap() const
{
    auto result = Item::toMap();
    result["done"] = m_done;
    result["todoUid"] = m_todoUid;
    return result;
}

void Task::fromMap(QVariantMap map)
{
    Item::fromMap(map);
    setDone(map.value("done", m_done).toBool());
    setTodoUid(map.value("todoUid", m_todoUid).toUuid());
}

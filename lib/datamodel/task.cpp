/*
 * Copyright 2020 Martin Hoeher <martin@rpdev.net>
 +
 * This file is part of OpenTodoList.
 *
 * OpenTodoList is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * OpenTodoList is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenTodoList.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "task.h"

Task::Task(QObject *parent) : Task(QString(), parent) {}

Task::Task(const QDir &dir, QObject *parent) : Item(dir, parent), m_done(false), m_todoUid()
{
    connect(this, &Task::doneChanged, this, &Task::changed);
    connect(this, &Task::todoUidChanged, this, &Task::changed);
}

Task::Task(const QString &filename, QObject *parent)
    : Item(filename, parent), m_done(false), m_todoUid(QUuid())
{
    connect(this, &Task::doneChanged, this, &Task::changed);
    connect(this, &Task::todoUidChanged, this, &Task::changed);
}

Task::~Task() {}

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

void Task::setTodoUid(const QUuid &todoUid)
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

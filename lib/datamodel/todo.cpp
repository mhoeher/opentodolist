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

#include "todo.h"

#include <QQmlEngine>

#include "library.h"
#include "task.h"

/**
 * @brief Constructor.
 */
Todo::Todo(const QString& filename, QObject* parent)
    : ComplexItem(filename, parent),
      m_todoListUid(),
      m_percentageDone(0),
      m_numSubtasks(0),
      m_numDoneSubtasks(0),
      m_progress(-1),
      m_done(false)
{
    connect(this, &Todo::todoListUidChanged, this, &ComplexItem::changed);
    connect(this, &Todo::doneChanged, this, &ComplexItem::changed);
    connect(this, &Todo::progressChanged, this, &ComplexItem::changed);
}

/**
 * @brief Constructor.
 */
Todo::Todo(QObject* parent) : Todo(QString(), parent) {}

/**
 * @brief Constructor.
 */
Todo::Todo(const QDir& dir, QObject* parent)
    : ComplexItem(dir, parent), m_todoListUid(), m_percentageDone(0), m_progress(-1), m_done(false)
{
    connect(this, &Todo::todoListUidChanged, this, &ComplexItem::changed);
    connect(this, &Todo::doneChanged, this, &ComplexItem::changed);
    connect(this, &Todo::progressChanged, this, &ComplexItem::changed);
}

/**
 * @brief Destructor.
 */
Todo::~Todo() {}

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
        if (isRecurring() && done) {
            markCurrentOccurrenceAsDone();
        } else {
            m_done = done;
        }
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
    if (m_progress > -1) {
        return m_progress;
    } else {
        return m_percentageDone;
    }
}

void Todo::applyCalculatedProperties(const QVariantMap& properties)
{
    ComplexItem::applyCalculatedProperties(properties);
    auto percentage = properties.value("percentageDone", m_percentageDone).toInt();
    if (percentage != m_percentageDone) {
        m_percentageDone = percentage;
        emit percentageDoneChanged();
    }
    setNumSubtasks(properties.value("numSubtasks", 0).toInt());
    setNumDoneSubtasks(properties.value("numDoneSubtasks", 0).toInt());
}

Item* Todo::copyTo(const QDir& targetDirectory, const QUuid& targetLibraryUuid,
                   const QUuid& targetItemUid)
{
    auto result = ComplexItem::copyTo(targetDirectory, targetLibraryUuid, targetItemUid);
    auto todo = qobject_cast<Todo*>(result);
    if (todo) {
        todo->m_todoListUid = targetItemUid;
    }
    return result;
}

QVariantMap Todo::toMap() const
{
    auto result = ComplexItem::toMap();
    result["todoListUid"] = m_todoListUid;
    result["done"] = m_done;
    result["progress"] = m_progress;
    return result;
}

void Todo::fromMap(QVariantMap map)
{
    ComplexItem::fromMap(map);
    setTodoListUid(map.value("todoListUid", m_todoListUid).toUuid());
    setDone(map.value("done", m_done).toBool());
    setProgress(map.value("progress", -1).toInt());
}

int Todo::numDoneSubtasks() const
{
    return m_numDoneSubtasks;
}

void Todo::setNumDoneSubtasks(int newNumDoneSubtasks)
{
    if (m_numDoneSubtasks == newNumDoneSubtasks)
        return;
    m_numDoneSubtasks = newNumDoneSubtasks;
    emit numDoneSubtasksChanged();
}

int Todo::numSubtasks() const
{
    return m_numSubtasks;
}

void Todo::setNumSubtasks(int newNumSubtasks)
{
    if (m_numSubtasks == newNumSubtasks)
        return;
    m_numSubtasks = newNumSubtasks;
    emit numSubtasksChanged();
}

/**
 * @brief Implementation of ComplexItem::markItemAsDone().
 */
void Todo::markItemAsDone()
{
    m_done = true;
    emit doneChanged();
}

/**
 * @brief The progress of the todo.
 *
 * This value indicates the progress or state of completion of the todo.
 * The progress value can be manually set to a value between 0 and 100
 * where 0 means not yet started at all and 100 means completely done.
 *
 * Additionally, a value of -1 can be set.
 *
 * This this property is set to a value greater than -1, the
 * percentageDone() property will be equal to the progress value.
 */
int Todo::progress() const
{
    return m_progress;
}

/**
 * @brief Set the progress value of the todo.
 */
void Todo::setProgress(int progress)
{
    progress = qBound(-1, progress, 100);
    if (m_progress != progress) {
        m_progress = progress;
        emit progressChanged();
        emit percentageDoneChanged();
    }
}

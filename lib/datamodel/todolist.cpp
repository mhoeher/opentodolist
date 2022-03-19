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

#include "todolist.h"

#include <QQmlEngine>

#include "library.h"
#include "todo.h"
#include "fileutils.h"

/**
 * @brief Constructor.
 */
TodoList::TodoList(QObject* parent) : TodoList(QString(), parent) {}

/**
 * @brief Constructor.
 */
TodoList::TodoList(const QString& filename, QObject* parent)
    : TopLevelItem(filename, parent), m_percentageDone(0), m_numTodos(0), m_numDoneTodos(0)
{
}

/**
 * @brief Constructor.
 */
TodoList::TodoList(const QDir& dir, QObject* parent)
    : TopLevelItem(dir, parent), m_percentageDone(0), m_numTodos(0), m_numDoneTodos(0)
{
}

/**
 * @brief Destructor.
 */
TodoList::~TodoList() {}

void TodoList::applyCalculatedProperties(const QVariantMap& properties)
{
    ComplexItem::applyCalculatedProperties(properties);
    setEarliestChildDueTo(properties.value("earliestChildDueDate", QDateTime()).toDateTime());

    auto percentage = properties.value("percentageDone", m_percentageDone).toInt();
    if (percentage != m_percentageDone) {
        m_percentageDone = percentage;
        emit percentageDoneChanged();
    }
    setNumTodos(properties.value("numSubtasks", 0).toInt());
    setNumDoneTodos(properties.value("numDoneSubtasks", 0).toInt());
}

int TodoList::percentageDone() const
{
    return m_percentageDone;
}

/**
 * @brief Set the percentage this todo list is done.
 */
void TodoList::setPercentageDone(int newPercentageDone)
{
    if (m_percentageDone == newPercentageDone)
        return;
    m_percentageDone = newPercentageDone;
    emit percentageDoneChanged();
}

/**
 * @brief The total number of todos within this todo list.
 */
int TodoList::numTodos() const
{
    return m_numTodos;
}

/**
 * @brief Set the total number of todos within the todo list.
 */
void TodoList::setNumTodos(int newNumTodos)
{
    if (m_numTodos == newNumTodos)
        return;
    m_numTodos = newNumTodos;
    emit numTodosChanged();
}

/**
 * @brief The number of done todos within the todo list.
 */
int TodoList::numDoneTodos() const
{
    return m_numDoneTodos;
}

void TodoList::finishCloning(Item* source)
{
    ComplexItem::finishCloning(source);
    auto sourceList = qobject_cast<TodoList*>(source);
    if (sourceList) {
        setEarliestChildDueTo(sourceList->earliestChildDueTo());
        m_percentageDone = sourceList->m_percentageDone;
        m_numTodos = sourceList->m_numTodos;
        m_numDoneTodos = sourceList->m_numDoneTodos;
    }
}

/**
 * @brief Set the number of done todos within the todo list.
 */
void TodoList::setNumDoneTodos(int newNumDoneTodos)
{
    if (m_numDoneTodos == newNumDoneTodos)
        return;
    m_numDoneTodos = newNumDoneTodos;
    emit numDoneTodosChanged();
}

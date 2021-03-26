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

#include "problem.h"

#include <QVariant>

/**
 * @brief Constructor.
 */
Problem::Problem() : m_id(QUuid::createUuid()), m_type(InvalidProblem), m_contextObject(nullptr) {}

/**
 * @brief Destructor.
 */
Problem::~Problem() {}

/**
 * @brief A globally unique ID identifying the problem.
 */
QUuid Problem::id() const
{
    return m_id;
}

/**
 * @brief The type of problem.
 */
Problem::ProblemType Problem::type() const
{
    return m_type;
}

/**
 * @brief Set the problem @p type.
 */
void Problem::setType(const ProblemType& type)
{
    m_type = type;
}

/**
 * @brief The name of the problem type.
 */
QString Problem::typeName() const
{
    return QVariant::fromValue(m_type).toString();
}

/**
 * @brief The context object of the problem.
 *
 * This is the object which the problem refers to.
 */
QSharedPointer<QObject> Problem::contextObject() const
{
    return m_contextObject;
}

/**
 * @brief Set the @p contextObject of the problem.
 */
void Problem::setContextObject(const QSharedPointer<QObject>& contextObject)
{
    m_contextObject = contextObject;
}

/**
 * @brief The context object of the problem.
 *
 * This is the object which the problem refers to.
 */
QObject* Problem::getContextObject() const
{
    return m_contextObject.data();
}

/**
 * @brief An optional message, holding further information about the problem.
 */
QString Problem::message() const
{
    return m_message;
}

/**
 * @brief Set an optional message, holding further information about the problem.
 */
void Problem::setMessage(const QString& message)
{
    m_message = message;
}

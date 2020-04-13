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

#include "utils/problemmanager.h"

/**
 * @brief Constructor.
 */
ProblemManager::ProblemManager(QObject *parent) : QObject(parent), m_problems()
{
    qRegisterMetaType<Problem>();
}

/**
 * @brief The list of problems.
 */
QList<Problem> ProblemManager::problems() const
{
    return m_problems;
}

/**
 * @brief The list of problems.
 */
QVariantList ProblemManager::getProblems()
{
    QVariantList result;
    for (const auto &problem : m_problems) {
        result << QVariant::fromValue(problem);
    }
    return result;
}

/**
 * @brief Add a @p problem to the list of known problems.
 */
bool ProblemManager::addProblem(const Problem &problem)
{
    bool result = false;
    if (problem.type() != Problem::InvalidProblem) {
        m_problems << problem;
        emit problemsChanged();
        result = true;
    }
    return result;
}

/**
 * @brief Remove the problem from the list of problems.
 */
bool ProblemManager::removeProblem(const Problem &problem)
{
    for (int i = 0; i < m_problems.length(); ++i) {
        const auto &p = m_problems[i];
        if (p.id() == problem.id()) {
            m_problems.removeAt(i);
            emit problemsChanged();
            return true;
        }
    }
    return false;
}

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

#ifndef UTILS_PROBLEMMANAGER_H_
#define UTILS_PROBLEMMANAGER_H_

#include <QList>
#include <QObject>
#include <QVariantList>

#include "problem.h"

/**
 * @brief Manage problems detected in the application.
 */
class ProblemManager : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QVariantList problems READ getProblems NOTIFY problemsChanged)

public:
    explicit ProblemManager(QObject *parent = nullptr);

    QList<Problem> problems() const;
    QVariantList getProblems();
    bool addProblem(const Problem &problem);
    bool removeProblem(const Problem &problem);

signals:

    /**
     * @brief The list of problems changed.
     */
    void problemsChanged();

private:
    QList<Problem> m_problems;
};

#endif // UTILS_PROBLEMMANAGER_H_

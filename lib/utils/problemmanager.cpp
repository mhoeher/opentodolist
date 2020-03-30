#include "problemmanager.h"

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

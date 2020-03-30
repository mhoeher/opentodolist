#ifndef PROBLEMMANAGER_H
#define PROBLEMMANAGER_H

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

#endif // PROBLEMMANAGER_H

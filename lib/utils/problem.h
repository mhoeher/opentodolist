#ifndef PROBLEM_H
#define PROBLEM_H

#include <QObject>
#include <QSharedPointer>
#include <QUuid>

/**
 * @brief Represents a problem detected by the application.
 *
 * This class is used to encapsulate information related to a specific problem/issue detected in
 * the app. An example is e.g. that a password for an account is missing and hence the user
 * needs to enter the credentials again.
 */
class Problem
{
    Q_GADGET

    Q_PROPERTY(QUuid id READ id CONSTANT)
    Q_PROPERTY(QString type READ typeName CONSTANT)
    Q_PROPERTY(QObject *contextObject READ getContextObject CONSTANT)

public:
    /**
     * @brief The type of problem detected.
     */
    enum ProblemType {
        InvalidProblem, //!< Indicates that the problem instance is not valid.
        AccountSecretsMissing //!< Secrets for an account are missing.
    };

    Q_ENUM(ProblemType)

    Problem();
    virtual ~Problem();

    QUuid id() const;

    ProblemType type() const;
    void setType(const ProblemType &type);

    QString typeName() const;

    QSharedPointer<QObject> contextObject() const;
    void setContextObject(const QSharedPointer<QObject> &contextObject);

    QObject *getContextObject() const;

    QVariantMap problemTypes() const;

private:
    QUuid m_id;
    ProblemType m_type;
    QSharedPointer<QObject> m_contextObject;
};

Q_DECLARE_METATYPE(Problem)

#endif // PROBLEM_H

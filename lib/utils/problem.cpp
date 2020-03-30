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
void Problem::setType(const ProblemType &type)
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
void Problem::setContextObject(const QSharedPointer<QObject> &contextObject)
{
    m_contextObject = contextObject;
}

/**
 * @brief The context object of the problem.
 *
 * This is the object which the problem refers to.
 */
QObject *Problem::getContextObject() const
{
    return m_contextObject.data();
}

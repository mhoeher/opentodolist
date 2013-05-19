#include "abstracttodo.h"

/**
 * @class AbstractTodo
 * @brief Base class for all todos
 *
 * This is the base class for all todos. Depending on the platform, this
 * class get's subclassed to provide e.g. platform specific storage.
 */

/**
 * @brief Constructor
 * @param parent The parent object to use for the todo
 */
AbstractTodo::AbstractTodo(QObject *parent) :
    QObject(parent)
{
}

int AbstractTodo::priority() const
{
    return m_priority;
}

void AbstractTodo::setPriority(int priority)
{
    m_priority = priority;
}

int AbstractTodo::progress() const
{
    return m_progress;
}

void AbstractTodo::setProgress(int progress)
{
    m_progress = progress;
}

QString AbstractTodo::description() const
{
    return m_description;
}

void AbstractTodo::setDescription(const QString &description)
{
    m_description = description;
}

QString AbstractTodo::title() const
{
    return m_title;
}

void AbstractTodo::setTitle(const QString &title)
{
    m_title = title;
}

#include "backend.h"

namespace OpenTodoList {
namespace DataModel {

Backend::Backend(QObject *parent) :
    QObject(parent),
    m_hasId( false ),
    m_id( -1 ),
    m_name(),
    m_title(),
    m_description()
{
}

bool Backend::hasId() const
{
    return m_hasId;
}

int Backend::id() const
{
    return m_id;
}

void Backend::setId(int id)
{
    m_hasId = true;
    if ( m_id != id ) {
        m_id = id;
        emit idChanged();
    }
}

QString Backend::name() const
{
    return m_name;
}

void Backend::setName(const QString &name)
{
    if ( m_name != name ) {
        m_name = name;
        emit nameChanged();
    }
}

QString Backend::title() const
{
    return m_title;
}

void Backend::setTitle(const QString &title)
{
    if ( m_title != title ) {
        m_title = title;
        emit titleChanged();
    }
}

QString Backend::description() const
{
    return m_description;
}

void Backend::setDescription(const QString &description)
{
    if ( m_description != description ) {
        m_description = description;
        emit descriptionChanged();
    }
}

/**
   @brief Clones the Backend

   This returns a copy of the Backend object, which is owned by @p parent.
 */
Backend *Backend::clone(QObject *parent) const
{
    Backend *result = new Backend( parent );
    result->m_description   = m_description;
    result->m_hasId         = m_hasId;
    result->m_id            = m_id;
    result->m_name          = m_name;
    result->m_title         = m_title;
}

} // namespace DataModel
} // namespace OpenTodoList

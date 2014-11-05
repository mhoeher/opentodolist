#include "task.h"

namespace OpenTodoList {

namespace DataModel {

Task::Task(QObject *parent) :
    QObject(parent),
    m_uuid(),
    m_title(),
    m_done(),
    m_weigth( 0.0 ),
    m_metaAttributes(),
    m_todoUuid()
{
}

QUuid Task::uuid() const
{
    return m_uuid;
}

void Task::setUuid(const QUuid &uuid)
{
    m_uuid = uuid;
}

QString Task::title() const
{
    return m_title;
}

void Task::setTitle(const QString &title)
{
    if ( m_title != title ) {
        m_title = title;
        emit titleChanged();
    }
}

bool Task::done() const
{
    return m_done;
}

void Task::setDone(bool done)
{
    if ( m_done != done ) {
        m_done = done;
        emit doneChanged();
    }
}

double Task::weight() const
{
    return m_weigth;
}

void Task::setWeight(double weight)
{
    if ( m_weigth != weight ) {
        m_weigth = weight;
        emit weightChanged();
    }
}

QVariantMap Task::metaAttributes() const
{
    return m_metaAttributes;
}

void Task::setMetaAttributes(const QVariantMap &metaAttributes)
{
    m_metaAttributes = metaAttributes;
    emit metaAttributesChanged();
}

QUuid Task::todoUuid() const
{
    return m_todoUuid;
}

void Task::setTodoUuid(const QUuid &uuid)
{
    if ( m_todoUuid != uuid ) {
        m_todoUuid = uuid;
    }
}

} // namespace DataModel
} // namespace OpenTodoList

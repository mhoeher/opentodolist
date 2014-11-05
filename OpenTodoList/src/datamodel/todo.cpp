/*
 *  OpenTodoList - A todo and task manager
 *  Copyright (C) 2013, 2014  Martin Höher <martin@rpdev.net>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "todo.h"

#include "todolistlibrary.h"

// Todo: Remove this
#include "database/storagequery.h"

#include <QDebug>

namespace OpenTodoList {

namespace DataModel {

/**
   @brief Creates a new Todo
 */
Todo::Todo(QObject *parent) :
    QObject( parent ),
    m_uuid( QUuid() ),
    m_weight(0.0),
    m_done( false ),
    m_priority( -1 ),
    m_todoListUuid( QUuid() ),
    m_dueDate( QDateTime() ),
    m_title( QString() ),
    m_description( QString() ),
    m_deleted( false ),
    m_metaAttributes( QVariantMap() ),
    m_lastModificationTime( QDateTime() )
{
    setupTodo();
}

/**
   @brief Destructor
 */
Todo::~Todo()
{
}

const QUuid &Todo::uuid() const
{
    return m_uuid;
}

void Todo::setUuid(const QUuid &uuid)
{
    // TODO: How to handle this properly?
    if ( m_uuid != uuid ) {
        m_uuid = uuid;
        emit uuidChanged();
    }
}

double Todo::weight() const
{
    return m_weight;
}

void Todo::setWeight(double weight)
{
    if ( m_weight != weight ) {
        m_weight = weight;
        emit weightChanged();
    }
}

bool Todo::done() const
{
    return m_done;
}

void Todo::setDone(bool done)
{
    if ( m_done != done ) {
        m_done = done;
        emit doneChanged();
    }
}

int Todo::priority() const
{
    return qBound( -1, m_priority, 10 );
}

void Todo::setPriority(int priority)
{
    if ( m_priority != priority ) {
        m_priority = priority;
        emit priorityChanged();
    }
}

const QUuid &Todo::todoListUuid() const
{
    return m_todoListUuid;
}

void Todo::setTodoListUuid(const QUuid &uuid)
{
    // TODO: How to handle this properly?
    if ( m_todoListUuid != uuid ) {
        m_todoListUuid = uuid;
    }
}

const QDateTime &Todo::dueDate() const
{
    return m_dueDate;
}

void Todo::setDueDate(const QDateTime &dueOn)
{
    if ( m_dueDate != dueOn ) {
        m_dueDate = dueOn;
        emit dueDateChanged();
    }
}

const QString &Todo::title() const
{
    return m_title;
}

void Todo::setTitle(const QString &title)
{
    if ( m_title != title ) {
        m_title = title;
        emit titleChanged();
    }
}

const QString &Todo::description() const
{
    return m_description;
}

void Todo::setDescription(const QString &description)
{
    if ( m_description != description ) {
        m_description = description;
        emit descriptionChanged();
    }
}

bool Todo::isDeleted() const
{
    return m_deleted;
}

void Todo::setDeleted(bool isDeleted)
{
    if ( m_deleted != isDeleted ) {
        m_deleted = isDeleted;
        emit deletedChanged();
    }
}

QVariantMap Todo::metaAttributes() const
{
    return m_metaAttributes;
}

void Todo::setMetaAttributes(const QVariantMap &metaAttributes)
{
    m_metaAttributes = metaAttributes;
}

QDateTime Todo::lastModificationTime() const
{
    return m_lastModificationTime;
}

void Todo::setLastModificationTime(const QDateTime &dateTime)
{
    if ( m_lastModificationTime != dateTime ) {
        m_lastModificationTime = dateTime;
        emit lastModificationTimeChanged();
    }
}

/**
   @brief Toggles the state of the todo between "done" and "not done"

   @sa setProgress
 */
void Todo::toggle()
{
    setDone( !done() );
}

void Todo::setupTodo()
{
    connect( this, SIGNAL(weightChanged()), this, SIGNAL(changed()) );
    connect( this, SIGNAL(doneChanged()), this, SIGNAL(changed()) );
    connect( this, SIGNAL(priorityChanged()), this, SIGNAL(changed()) );
    connect( this, SIGNAL(dueDateChanged()), this, SIGNAL(changed()) );
    connect( this, SIGNAL(titleChanged()), this, SIGNAL(changed()) );
    connect( this, SIGNAL(descriptionChanged()), this, SIGNAL(changed()) );
    connect( this, SIGNAL(deletedChanged()), this, SIGNAL(changed()) );

    connect( this, SIGNAL(reset()), this, SIGNAL(weightChanged()) );
    connect( this, SIGNAL(reset()), this, SIGNAL(doneChanged()) );
    connect( this, SIGNAL(reset()), this, SIGNAL(priorityChanged()) );
    connect( this, SIGNAL(reset()), this, SIGNAL(dueDateChanged()) );
    connect( this, SIGNAL(reset()), this, SIGNAL(titleChanged()) );
    connect( this, SIGNAL(reset()), this, SIGNAL(descriptionChanged()) );
    connect( this, SIGNAL(reset()), this, SIGNAL(deletedChanged()) );
}

} /* DataModel */

} /* OpenTodoList */

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
    m_previousProgress( -1 ),
    m_uuid( QUuid() ),
    m_weight(0.0),
    m_progress( 0 ),
    m_priority( -1 ),
    m_parentUuid( QUuid() ),
    m_todoListUuid( QUuid() ),
    m_dueDate( QDateTime() ),
    m_title( QString() ),
    m_description( QString() ),
    m_deleted( false ),
    m_metaAttributes( QVariantMap() ),
    m_lastModificationTime( QDateTime() ),
    m_isPermanentlyDeleted( false )
{
    setupTodo();
}

/**
   @brief Destructor
 */
Todo::~Todo()
{
}

/**
   @brief Is the todo completed?

   Returns true if the todo is completed (i.e. the progress is 100%) or
   false otherwise.
 */
bool Todo::isDone() const
{
    return m_progress == 100;
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

int Todo::progress() const
{
    return qBound( 0, m_progress, 100 );
}

void Todo::setProgress(int progress)
{
    if ( m_progress != progress ) {
        m_previousProgress = m_progress;
        m_progress = progress;
        emit progressChanged();
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

const QUuid &Todo::parentTodoUuid() const
{
    return m_parentUuid;
}

void Todo::setParentTodoUuid(const QUuid &uuid)
{
    // TODO: How to handle this properly?
    if ( m_parentUuid != uuid ) {
        m_parentUuid = uuid;
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

const QStringList Todo::metaAttributeKeys() const
{
    return m_metaAttributes.keys();
}

const QVariant Todo::metaAttribute(const QString &key) const
{
    return m_metaAttributes.value( key );
}

void Todo::setMetaAttribute(const QString &key, const QVariant &value)
{
    // TODO: How to handle this properly?
    m_metaAttributes.insert( key, value );
}

bool Todo::hasMetaAttribute(const QString &key) const
{
    return m_metaAttributes.contains( key );
}

void Todo::deleteMetaAttribute(const QString &key)
{
    m_metaAttributes.remove( key );
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

bool Todo::disposed() const
{
    return m_isPermanentlyDeleted;
}

/**
   @brief Toggles the state of the todo between "done" and "not done"

   @sa setProgress
 */
void Todo::toggle()
{
    if ( m_progress == 100 ) {
        setProgress( m_previousProgress >= 0 && m_previousProgress < 100 ?
                         m_previousProgress : 0 );
    } else {
        setProgress( 100 );
    }
}

void Todo::setupTodo()
{
    connect( this, SIGNAL(weightChanged()), this, SIGNAL(changed()) );
    connect( this, SIGNAL(progressChanged()), this, SIGNAL(changed()) );
    connect( this, SIGNAL(priorityChanged()), this, SIGNAL(changed()) );
    connect( this, SIGNAL(dueDateChanged()), this, SIGNAL(changed()) );
    connect( this, SIGNAL(titleChanged()), this, SIGNAL(changed()) );
    connect( this, SIGNAL(descriptionChanged()), this, SIGNAL(changed()) );
    connect( this, SIGNAL(deletedChanged()), this, SIGNAL(changed()) );

    connect( this, SIGNAL(reset()), this, SIGNAL(weightChanged()) );
    connect( this, SIGNAL(reset()), this, SIGNAL(progressChanged()) );
    connect( this, SIGNAL(reset()), this, SIGNAL(priorityChanged()) );
    connect( this, SIGNAL(reset()), this, SIGNAL(dueDateChanged()) );
    connect( this, SIGNAL(reset()), this, SIGNAL(titleChanged()) );
    connect( this, SIGNAL(reset()), this, SIGNAL(descriptionChanged()) );
    connect( this, SIGNAL(reset()), this, SIGNAL(deletedChanged()) );
    connect( this, SIGNAL(reset()), this, SIGNAL(canCreateTodosChanged()) );
}

} /* DataModel */

} /* OpenTodoList */

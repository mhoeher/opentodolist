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

#include "datamodel/todolist.h"
#include "datamodel/todolistlibrary.h"

// TODO: Remove this
#include "database/storagequery.h"

namespace OpenTodoList {

namespace DataModel {

/**
   @brief Creates a new TodoList
 */
TodoList::TodoList(QObject *parent) :
    QObject( parent ),
    m_uuid( QUuid() ),
    m_name( QString() ),
    m_metaAttributes(),
    m_deleted( false ),
    m_disposed( false ),
    m_lastModificationTime()

{
    connect( this, SIGNAL(nameChanged()), this, SIGNAL(changed()) );
    connect( this, SIGNAL(deletedChanged()), this, SIGNAL(changed()) );
}

/**
   @brief Destructor
 */
TodoList::~TodoList()
{
}

const QUuid &TodoList::uuid() const
{
    return m_uuid;
}

void TodoList::setUuid(const QUuid &uuid)
{
    // TODO: How to handle this properly?
    if ( m_uuid != uuid ) {
        m_uuid = uuid;
    }
}

const QString &TodoList::name() const
{
    return m_name;
}

void TodoList::setName(const QString &name)
{
    if ( m_name != name ) {
        m_name = name;
        emit nameChanged();
    }
}

const QStringList TodoList::metaAttributeKeys() const
{
    return m_metaAttributes.keys();
}

const QVariant TodoList::metaAttribute(const QString &key) const
{
    return m_metaAttributes.value( key );
}

void TodoList::setMetaAttribute(const QString &key, const QVariant &value)
{
    m_metaAttributes.insert( key, value );
}

bool TodoList::hasMetaAttribute(const QString &key) const
{
    return m_metaAttributes.contains( key );
}

void TodoList::deleteMetaAttribute(const QString &key)
{
    m_metaAttributes.remove( key );
}

bool TodoList::deleted() const
{
    return m_deleted;
}

void TodoList::setDeleted(bool deleted)
{
    if ( m_deleted != deleted ) {
        m_deleted = deleted;
        emit deletedChanged();
    }
}

bool TodoList::disposed() const
{
    return m_disposed;
}

QDateTime TodoList::lastModificationTime() const
{
    return m_lastModificationTime;
}

void TodoList::setLastModificationTime(const QDateTime &dateTime)
{
    if ( m_lastModificationTime != dateTime ) {
        m_lastModificationTime = dateTime;
    }
}

} /* DataModel */

} /* OpenTodoList */

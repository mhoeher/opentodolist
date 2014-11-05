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

QVariantMap TodoList::metaAttributes() const
{
    return m_metaAttributes;
}

void TodoList::setMetaAttributes(const QVariantMap &metaAttributes)
{
    m_metaAttributes = metaAttributes;
    emit metaAttributesChanged();
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

QUuid TodoList::accountUuid() const
{
    return m_accountUuid;
}

void TodoList::setAccountUuid(const QUuid &uuid)
{
    if ( m_accountUuid != uuid ) {
        m_accountUuid = uuid;
    }
}

} /* DataModel */

} /* OpenTodoList */

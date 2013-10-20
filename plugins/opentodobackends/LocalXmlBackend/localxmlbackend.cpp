/*
 *  OpenTodoList - A todo and task manager
 *  Copyright (C) 2013  Martin Höher <martin@rpdev.net>
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

#include "localxmlbackend.h"

#include "localxmltodolistfactory.h"

#include <QtPlugin>

LocalXmlBackend::LocalXmlBackend(QObject *parent) :
    OpenTodoListBackend(parent),
    m_factory( 0 )
{
    setName( tr( "Local XML Directory" ) );
    setDescription( tr( "Stores data in XML files located in a directory on your local host." ) );
    setType( "LocalXmlDirectory" );
    
    m_factory = new LocalXmlTodoListFactory( type(), this );
}

TodoListFactory *LocalXmlBackend::factory()
{
    return m_factory;
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(LocalXmlBackend, LocalXmlBackend)
#endif // QT_VERSION < 0x050000

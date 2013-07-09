/*
 *  LocalXmlBackend - Local XML storage for OpenTodoList
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


#include "localxmltodo.h"

#include "localxmltodolist.h"

#include <QDomDocument>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QString>
#include <QTextStream>
#include <QTimer>

LocalXmlTodo::LocalXmlTodo(const QString& key, AbstractTodoList* parent): 
    AbstractTodo( parent ),
    m_key( key ),
    m_parentKey( QString() )
{
    load();
    connect( this, SIGNAL(changed()), this, SLOT(save()) );
}

LocalXmlTodo::~LocalXmlTodo()
{
}

void LocalXmlTodo::save()
{
    // TODO: If XML already exists, update!
    QFileInfo fi( m_key );
    QDir dir = fi.absoluteDir();
    if ( dir.exists() || dir.mkpath( dir.absolutePath() ) ) {
        QDomDocument xml;
        QDomProcessingInstruction xmlDeclaration =
                xml.createProcessingInstruction(
                    "xml", "version=\"1.0\" encoding=\"utf-8\"");
        xml.appendChild(xmlDeclaration);
        QDomElement root = xml.createElement( "todo" );
        xml.appendChild( root );
        root.setAttribute( "title", title() );
        root.setAttribute( "progress", QString::number( progress() ) );
        root.setAttribute( "priority", QString::number( priority() ) );
        root.setAttribute( "deleted", isDeleted() ? "true" : "false" );
        if ( dueDate().isValid() ) {
            root.setAttribute( "dueDate", dueDate().toString() );
        } else {
            root.removeAttribute( "dueDate" );
        }
        LocalXmlTodo* parentTodoItem = qobject_cast< LocalXmlTodo* >( parentTodo() );
        if ( parentTodoItem ) {
            root.setAttribute( "parent", parentTodoItem->m_key );
        }
        QDomElement descriptionElement = xml.createElement( "description" );
        QDomText descriptionText = xml.createTextNode( description() );
        descriptionElement.appendChild( descriptionText );
        root.appendChild( descriptionElement );
        QFile file( m_key );
        if ( file.open( QIODevice::WriteOnly ) ) {
            QTextStream stream( &file );
            stream << xml.toString();
            file.close();
        }
    }
}

void LocalXmlTodo::load()
{
    QFile file( m_key );
    if ( file.open( QIODevice::ReadOnly ) ) {
        QDomDocument xml;
        if ( xml.setContent( file.readAll() ) ) {
            QDomElement root = xml.documentElement();
            setTitle( root.attribute( "title" ) );
            setProgress( root.attribute( "progress", "0" ).toInt() );
            setPriority( root.attribute( "priority", "-1" ).toInt() );
            setDeleted( root.attribute( "deleted", "false" ) == "true" );
            if ( !root.hasAttribute( "dueDate" ) ) {
                setDueDate( QDateTime() );
            } else {
                setDueDate( QDateTime::fromString( root.attribute( "dueDate" ) ) );
            }
            m_parentKey = root.attribute( "parent", QString() );
            if ( !m_parentKey.isEmpty() ) {
                QTimer::singleShot( 0, this, SLOT(updateParentTodo() ) );
            }
            QDomNodeList descriptionElements = root.elementsByTagName( "description" );
            if ( descriptionElements.size() > 0 ) {
                setDescription( descriptionElements.at( 0 ).toElement().text() );
            }
        }
        file.close();
    }
}

void LocalXmlTodo::updateParentTodo()
{
    if ( !m_parentKey.isEmpty() ) {
        foreach ( AbstractTodo* todo, parent()->todos()->data() ) {
            LocalXmlTodo* xmlTodo = qobject_cast< LocalXmlTodo* >( todo );
            if ( xmlTodo ) {
                if ( xmlTodo->m_key == m_parentKey ) {
                    setParentTodo( xmlTodo );
                }
            }
        }
    }
}

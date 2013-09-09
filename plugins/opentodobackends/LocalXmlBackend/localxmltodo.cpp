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

LocalXmlTodo::LocalXmlTodo(QUuid id, const QString& configFile, AbstractTodoList* parent): 
    AbstractTodo( id, parent ),
    m_configFile( configFile ),
    m_parentId( QUuid() )
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
    QFileInfo fi( m_configFile );
    QDir dir = fi.absoluteDir();
    if ( dir.exists() || dir.mkpath( dir.absolutePath() ) ) {
        QDomDocument xml;
        QDomProcessingInstruction xmlDeclaration =
                xml.createProcessingInstruction(
                    "xml", "version=\"1.0\" encoding=\"utf-8\"");
        xml.appendChild(xmlDeclaration);
        QDomElement root = xml.createElement( "todo" );
        xml.appendChild( root );
        root.setAttribute( "id", id().toString() );
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
            root.setAttribute( "parent", parentTodoItem->id().toString() );
        }
        QDomElement descriptionElement = xml.createElement( "description" );
        QDomText descriptionText = xml.createTextNode( description() );
        descriptionElement.appendChild( descriptionText );
        root.appendChild( descriptionElement );
        QFile file( m_configFile );
        if ( file.open( QIODevice::WriteOnly ) ) {
            QTextStream stream( &file );
            stream.setCodec( "UTF-8" );
            stream << xml.toString().toUtf8();
            file.close();
        }
    }
}

void LocalXmlTodo::load()
{
    QFile file( m_configFile );
    if ( file.open( QIODevice::ReadOnly ) ) {
        QDomDocument xml;
        if ( xml.setContent( file.readAll() ) ) {
            QDomElement root = xml.documentElement();
            if ( root.hasAttribute( "id" ) ) {
                setId( QUuid( root.attribute( "id" ) ) );
            } else {
                setId( QUuid::createUuid() ); // Import from old format - generate new ID
                QTimer::singleShot( 0, this, SLOT(save()) ); // to make sure we write back our ID immediately :)
            }
            setTitle( root.attribute( "title" ) );
            setProgress( root.attribute( "progress", "0" ).toInt() );
            setPriority( root.attribute( "priority", "-1" ).toInt() );
            setDeleted( root.attribute( "deleted", "false" ) == "true" );
            if ( !root.hasAttribute( "dueDate" ) ) {
                setDueDate( QDateTime() );
            } else {
                setDueDate( QDateTime::fromString( root.attribute( "dueDate" ) ) );
            }
            QString parentIdString = root.attribute( "parent" );
            if ( !parentIdString.isEmpty() ) {
                QUuid parentId( parentIdString );
                if ( parentId.isNull() ) {
                    // Import from previous implementation
                    m_parentConfigFile = parentIdString;
                } else {
                    m_parentId = parentId;
                }
            }
            if ( !m_parentId.isNull() || !m_parentConfigFile.isEmpty() ) {
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
    if ( !m_parentId.isNull() || !m_parentConfigFile.isEmpty() ) {
        foreach ( AbstractTodo* todo, parent()->todos()->data() ) {
            LocalXmlTodo* xmlTodo = qobject_cast< LocalXmlTodo* >( todo );
            if ( xmlTodo ) {
                if ( !m_parentId.isNull() && m_parentId == xmlTodo->id() ) {
                    setParentTodo( xmlTodo );
                    return;
                }
                if ( !m_parentConfigFile.isEmpty() && xmlTodo->m_configFile == m_parentConfigFile ) {
                    setParentTodo( xmlTodo );
                    return;
                }
            }
        }
    }
}

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

#include "localxmltodolist.h"

#include "localxmltodo.h"

#include <QDir>
#include <QFile>
#include <QDomDocument>
#include <QTextStream>

LocalXmlTodoList::LocalXmlTodoList(const QString& key,
                                   const QString& type,
                                   QVariant settings,
                                   QString localStorageLocation,
                                   QObject* parent):
    AbstractTodoList(key, type, settings.toMap().value( "baseSettings" ), parent)
{
    QVariantMap map = settings.toMap();
    if ( map.contains( "dir" ) ) {
        m_dir = map.value( "dir" ).toString();
    } else {
        m_dir = localStorageLocation + "/" + key;
    }
    load();
    connect( this, SIGNAL(changed()), this, SLOT(save()) );
}

LocalXmlTodoList::~LocalXmlTodoList()
{
}

QObject* LocalXmlTodoList::addTodo()
{
    QUuid id = QUuid::createUuid();
    QString todoFile = m_dir + "/todos/" + id.toString() + ".xml";
    LocalXmlTodo* todo = new LocalXmlTodo( id, todoFile, this );
    appendTodo( todo );
    return todo;
}

QVariant LocalXmlTodoList::settings()
{
    QVariantMap result;
    result.insert( "dir", m_dir );
    QVariant baseSettings = AbstractTodoList::settings();
    if ( baseSettings.isValid() ) {
        result.insert( "baseSettings", baseSettings );
    }
    return result;
}

void LocalXmlTodoList::save()
{
    //TODO: If XML already exists, update
    QDir dir( m_dir );
    if ( !dir.exists() ) {
        dir.mkpath( dir.absolutePath() );
    }
    QDomDocument xml;
    QDomProcessingInstruction xmlDeclaration = xml.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"utf-8\"");
    xml.appendChild(xmlDeclaration);
    QDomElement root = xml.createElement( "todoList" );
    xml.appendChild( root );
    root.setAttribute( "name", name() );
    QFile file( m_dir + "/config.xml" );
    if ( file.open( QIODevice::WriteOnly ) ) {
        QTextStream stream( &file );
        stream.setCodec( "UTF-8" );
        stream << xml.toString();
        file.close();
    }
}

void LocalXmlTodoList::load()
{
    QFile file( m_dir + "/config.xml" );
    if ( file.exists() && file.open( QIODevice::ReadOnly ) ) {
        QDomDocument xml;
        if ( xml.setContent( file.readAll() ) ) {
            setName( xml.documentElement().attribute( "name" ) );
            QDir dir( m_dir + "/todos" );
            if ( dir.exists() ) {
                foreach ( QString entry, dir.entryList(
                              QStringList() << "*.xml", QDir::Files ) ) {
                    LocalXmlTodo* todo = new LocalXmlTodo(
                                QUuid(), // going to be set to proper value when restoring
                                dir.absolutePath() + "/" + entry,
                                this );
                    appendTodo( todo );
                }
            }
        }
        file.close();
    }
}

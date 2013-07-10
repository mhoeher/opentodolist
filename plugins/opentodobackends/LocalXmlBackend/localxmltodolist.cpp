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

#include "localxmltodolist.h"

#include "localxmltodo.h"

#include <QDir>
#include <QFile>
#include <QDomDocument>
#include <QTextStream>

LocalXmlTodoList::LocalXmlTodoList(const QString& key, const QString& type, QObject* parent): 
    AbstractTodoList(key, type, parent)
{
    load();
    connect( this, SIGNAL(changed()), this, SLOT(save()) );
}

LocalXmlTodoList::~LocalXmlTodoList()
{
}

QObject* LocalXmlTodoList::addTodo()
{
    int i = 0;
    QString todoKey;
    while ( QFile::exists( todoKey = key() + QString( "/todos/%1.xml" ).arg( i ) ) ) {
        ++i;
    }
    LocalXmlTodo* todo = new LocalXmlTodo( todoKey, this );
    appendTodo( todo );
    return todo;
}

void LocalXmlTodoList::save()
{
    //TODO: If XML already exists, update
    QDir dir( key() );
    if ( !dir.exists() ) {
        dir.mkpath( dir.absolutePath() );
    }
    QDomDocument xml;
    QDomProcessingInstruction xmlDeclaration = xml.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"utf-8\"");
    xml.appendChild(xmlDeclaration);
    QDomElement root = xml.createElement( "todoList" );
    xml.appendChild( root );
    root.setAttribute( "name", name() );
    QFile file( key() + "/config.xml" );
    if ( file.open( QIODevice::WriteOnly ) ) {
        QTextStream stream( &file );
        stream.setCodec( "UTF-8" );
        stream << xml.toString();
        file.close();
    }
}

void LocalXmlTodoList::load()
{
    QFile file( key() + "/config.xml" );
    if ( file.exists() && file.open( QIODevice::ReadOnly ) ) {
        QDomDocument xml;
        if ( xml.setContent( file.readAll() ) ) {
            setName( xml.documentElement().attribute( "name" ) );
            QDir dir( key() + "/todos" );
            if ( dir.exists() ) {
                foreach ( QString entry, dir.entryList( 
                    QStringList() << "*.xml", QDir::Files ) ) {
                    LocalXmlTodo* todo = new LocalXmlTodo( 
                    dir.absolutePath() + "/" + entry, this );
                    appendTodo( todo );
                    }
            }
        }
        file.close();
    }
}

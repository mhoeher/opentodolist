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

#include <QDebug>
#include <QDirIterator>
#include <QDomDocument>
#include <QStringList>
#include <QtPlugin>

const QString LocalXmlBackend::TodoListConfigFileName = "config.xml";
const QString LocalXmlBackend::TodoDirectoryName = "todos";

LocalXmlBackend::LocalXmlBackend(QObject *parent) :
    QObject( parent ),
    m_database( 0 ),
    m_localStorageDirectory( QString() )
{

}

void LocalXmlBackend::setDatabase(TodoListDatabase *database)
{
    Q_ASSERT( m_database == 0 );
    m_database = database;
}

void LocalXmlBackend::setLocalStorageDirectory(const QString &directory)
{
    Q_ASSERT( m_localStorageDirectory.isNull() );
    m_localStorageDirectory = directory;
    qDebug() << "Set local storage directory of" << id() << "to" << directory;
}

QString LocalXmlBackend::id() const
{
    return "LocalXmlDirectory";
}

QString LocalXmlBackend::name() const
{
    return tr( "Todo List in local files (XML format)" );
}

QString LocalXmlBackend::description() const
{
    return tr( "Stores your todos locally in a directory using an "
               "XML file format." );
}

bool LocalXmlBackend::start()
{
    QStringList todoLists = locateTodoLists();
    foreach ( const QString &todoList, todoLists ) {
        TodoListStruct list = todoListFromFile( todoList );
        if ( !list.id.isNull() ) {
            m_database->insertTodoList( list );
            QStringList todos = locateTodos( todoList );
            double weight = 0.0;
            foreach ( const QString &todo, todos ) {
                TodoStruct todoStruct = todoFromFile( todo, weight );
                if ( !todoStruct.id.isNull() ) {
                    weight = todoStruct.weight + 1.0;
                    todoStruct.todoListId = list.id;
                    m_database->insertTodo( todoStruct );
                }
            }
        }
    }
    return true;
}

bool LocalXmlBackend::stop()
{
    return true;
}

bool LocalXmlBackend::notifyTodoListChanged(const TodoListStruct &list)
{
    return true;
}

bool LocalXmlBackend::notifyTodoChanged(const TodoStruct &todo)
{
    return true;
}

/**
   @brief Automatically locate todo lists

   Searches for all todo lists and returns them as a string list. Only lists
   relative to the local storage directory are returned.
 */
QStringList LocalXmlBackend::locateTodoLists() const
{
    QStringList result;
    QDirIterator it( m_localStorageDirectory, QDirIterator::Subdirectories );
    while ( it.hasNext() ) {
        it.next();
        QFileInfo currentFile = it.fileInfo();
        if ( currentFile.isFile() && currentFile.fileName() == TodoListConfigFileName ) {
            result << currentFile.absoluteFilePath();
        }
    }
    return result;
}

QStringList LocalXmlBackend::locateTodos(const QString &todoList) const
{
    QFileInfo fi( todoList );
    QDir dir = fi.dir();
    dir.cd( TodoDirectoryName );
    QStringList entries = dir.entryList( QStringList() << "*.xml", QDir::Files );
    QStringList result;
    foreach ( const QString &entry, entries ) {
        result << dir.absoluteFilePath( entry );
    }
    return result;
}

TodoListStruct LocalXmlBackend::todoListFromFile(const QString &fileName)
{
    TodoListStruct result = m_database->nullTodoList();

    QFile file( fileName );
    if ( file.open( QIODevice::ReadOnly ) ) {
        QDomDocument doc;
        QString errorMessage;
        int errorLine, errorColumn;
        if ( doc.setContent( &file, &errorMessage, &errorLine, &errorColumn ) ) {
            QDomElement root = doc.documentElement();
            if ( root.hasAttribute( "name" ) ) {
                result.name = root.attribute( "name" );
            }
            if ( root.hasAttribute( "id" ) ) {
                result.id = QUuid( root.attribute( "id" ) );
            } else {
                result.id = QUuid::createUuid();
            }
        } else {
            qWarning() << "Error reading todo list from" << fileName << ":"
                       << errorMessage << "in line" << errorLine << "in column"
                       << errorColumn;
        }
    } else {
        qWarning() << "Unable to open todo list file" << fileName << ":"
                   << file.errorString();
    }

    result.meta.insert( "LocalXmlBackend::TodoList::fileName", fileName );
    return result;
}

TodoStruct LocalXmlBackend::todoFromFile(const QString &fileName, double weight )
{
    TodoStruct result = m_database->nullTodo();

    QFile file( fileName );
    if ( file.open( QIODevice::ReadOnly ) ) {
        QDomDocument doc;
        QString errorString;
        int errorLine, errorColumn;
        if ( doc.setContent(  &file, &errorString, &errorLine, &errorColumn  ) ) {
            QDomElement root = doc.documentElement();
            if ( root.hasAttribute( "id" ) ) {
                result.id = root.attribute( "id" );
            } else {
                result.id = QUuid::createUuid();
            }
            result.title = root.attribute( "title" );
            result.progress = qBound( 0, root.attribute( "progress", "0" ).toInt(), 100 );
            result.priority = qBound( -1, root.attribute( "priority", "-1" ).toInt(), 10 );
            result.deleted = root.attribute( "deleted", "false" ) == "true";
            if ( root.hasAttribute( "dueDate" ) ) {
                result.dueDate = QDateTime::fromString( root.attribute( "dueDate" ) );
            }
            if ( root.hasAttribute( "parent" ) ) {
                result.parentTodoId = QUuid( root.attribute( "parent" ) );
            }
            result.weight = root.attribute( "weight", QString::number( weight ) ).toDouble();
            QDomElement description = root.firstChildElement( "description" );
            if ( description.isElement() ) {
                result.description = description.text();
            }
        } else {
            qWarning() << "Error loading todo from" << fileName << ":"
                       << errorString << "in line" << errorLine << "in column"
                       << errorColumn;
        }
    } else {
        qWarning() << "Unable to open todo file" << fileName << ":"
                   << file.errorString();
    }

    result.meta.insert( "LocalXmlBackend::Todo::fileName", fileName );
    return result;
}

bool LocalXmlBackend::todoListToFile(const TodoListStruct &todoList)
{

}

bool LocalXmlBackend::todoToFile(const TodoStruct &todo)
{

}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(LocalXmlBackend, LocalXmlBackend)
#endif // QT_VERSION < 0x050000

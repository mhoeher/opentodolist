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

const QString LocalXmlBackend::TodoListMetaFileName = "LocalXmlBackend::TodoList::fileName";
const QString LocalXmlBackend::TodoMetaFileName = "LocalXmlBackend::Todo::fileName";

LocalXmlBackend::LocalXmlBackend(QObject *parent) :
    QObject( parent ),
    m_database( 0 ),
    m_localStorageDirectory( QString() )
{
    qDebug() << "Creating LocalXmlBackend";
}

LocalXmlBackend::~LocalXmlBackend()
{
    qDebug() << "Deleting LocalXmlBackend";
}

void LocalXmlBackend::setDatabase(OpenTodoList::IDatabase *database)
{
    m_database = database;
}

void LocalXmlBackend::setLocalStorageDirectory(const QString &directory)
{
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
        OpenTodoList::ITodoList *list = todoListFromFile( todoList );
        if ( list ) {
            m_database->insertTodoList( list );
            QStringList todos = locateTodos( todoList );
            double weight = 0.0;
            foreach ( const QString &todo, todos ) {
                OpenTodoList::ITodo *todoObj = todoFromFile( todo, weight );
                if ( todoObj ) {
                    weight = todoObj->weight() + 1.0;
                    todoObj->setTodoListUuid( list->uuid() );
                    m_database->insertTodo( todoObj );
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

/*
bool LocalXmlBackend::notifyTodoListChanged(const TodoListStruct &list)
{
    return todoListToFile( list );
}

bool LocalXmlBackend::notifyTodoChanged(const TodoStruct &todo)
{
    return todoToFile( todo );
}

void LocalXmlBackend::notifyTodoListDeleted(const TodoListStruct &list)
{
    QString fileName = list.meta.value( TodoListMetaFileName, QString() ).toString();
    QFileInfo fi( fileName );
    if ( fi.exists() ) {
        QDir dir = fi.absoluteDir();
        dir.removeRecursively();
    }
}

void LocalXmlBackend::notifyTodoDeleted(const TodoStruct &todo)
{
    QString fileName = todo.meta.value( TodoMetaFileName, QString() ).toString();
    QFileInfo fi( fileName );
    if ( fi.exists() ) {
        QFile file( fileName );
        file.remove();
    }
}

bool LocalXmlBackend::canAddTodo(const TodoListStruct &list, const TodoStruct &todo)
{
    if ( list.id.isNull() ) {
        return false;
    }
    if ( todo.id.isNull() ) {
        return true;
    } else {
        if ( todo.todoListId == list.id ) {
            return true;
        }
    }
    return false;
}

void LocalXmlBackend::addTodo(TodoStruct newTodo, const TodoListStruct &list, const TodoStruct &todo)
{
    if ( canAddTodo( list, todo ) ) {
        newTodo.id = QUuid::createUuid();
        if ( !todo.id.isNull() ) {
            newTodo.parentTodoId = todo.id;
        }
        newTodo.todoListId = list.id;

        QString todoListFileName = list.meta.value( TodoListMetaFileName ).toString();
        QFileInfo fi( todoListFileName );
        QDir dir = fi.absoluteDir();
        if ( dir.exists() || dir.mkpath( dir.absolutePath() ) ) {
            if ( dir.cd( TodoDirectoryName ) || dir.mkdir( TodoDirectoryName ) ) {
                newTodo.meta.insert( TodoMetaFileName, dir.absoluteFilePath( newTodo.id.toString() + ".xml" ) );
                todoToFile( newTodo );
                m_database->insertTodo( newTodo );
            }
        }
    }
}

bool LocalXmlBackend::canAddTodoList()
{
    return true;
}

void LocalXmlBackend::addTodoList(TodoListStruct newList)
{
    newList.id = QUuid::createUuid();

    QDir dir( m_localStorageDirectory );
    if ( dir.mkdir( newList.id.toString() ) ) {
        dir.cd( newList.id.toString() );
        newList.meta.insert( TodoListMetaFileName, dir.absoluteFilePath( TodoListConfigFileName ) );
        if ( todoListToFile( newList ) ) {
            m_database->insertTodoList( newList );
        } else {
            qWarning() << "Failed to create new todo list in file system";
        }
    } else {
        qWarning() << "Unable to create directory for new todo list";
    }
}

*/

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
    QStringList result;
    if ( dir.cd( TodoDirectoryName ) ) {
        QStringList entries = dir.entryList( QStringList() << "*.xml", QDir::Files );
        foreach ( const QString &entry, entries ) {
            result << dir.absoluteFilePath( entry );
        }
    }
    return result;
}

OpenTodoList::ITodoList *LocalXmlBackend::todoListFromFile(const QString &fileName)
{
    OpenTodoList::ITodoList *result = m_database->createTodoList();
    if ( result ) {
        QFile file( fileName );
        if ( file.open( QIODevice::ReadOnly ) ) {
            QDomDocument doc;
            QString errorMessage;
            int errorLine, errorColumn;
            if ( doc.setContent( &file, &errorMessage, &errorLine, &errorColumn ) ) {
                if ( !domToList( doc, result ) ) {
                    qWarning() << "File" << fileName << "is not a valid todo list XML";
                    delete result;
                    return 0;
                }
            } else {
                qWarning() << "Error reading todo list from" << fileName << ":"
                           << errorMessage << "in line" << errorLine << "in column"
                           << errorColumn;
                delete result;
                return 0;
            }
        } else {
            qWarning() << "Unable to open todo list file" << fileName << ":"
                       << file.errorString();
            delete result;
            return 0;
        }

        result->setMetaAttribute( TodoListMetaFileName, fileName );

        // TODO: Remove me in final release:
        if ( result->uuid().isNull() ) {
            result->setUuid( QUuid::createUuid() );
            result->setLastModificationTime( QDateTime::currentDateTime() );
            todoListToFile( result );
        }

        return result;
    }
    return 0;
}

OpenTodoList::ITodo *LocalXmlBackend::todoFromFile(const QString &fileName, double weight )
{
    OpenTodoList::ITodo *result = m_database->createTodo();
    result->setWeight( weight );

    if ( result ) {
        QFile file( fileName );
        if ( file.open( QIODevice::ReadOnly ) ) {
            QDomDocument doc;
            QString errorString;
            int errorLine, errorColumn;
            if ( doc.setContent(  &file, &errorString, &errorLine, &errorColumn  ) ) {
                if ( !domToTodo( doc, result ) ) {
                    qWarning() << "File" << fileName << "is not a valid todo XML file";
                    delete result;
                    return 0;
                }
            } else {
                qWarning() << "Error loading todo from" << fileName << ":"
                           << errorString << "in line" << errorLine << "in column"
                           << errorColumn;
                delete result;
                return 0;
            }
        } else {
            qWarning() << "Unable to open todo file" << fileName << ":"
                       << file.errorString();
            delete result;
            return 0;
        }

        result->setMetaAttribute( TodoMetaFileName, fileName );
        return result;
    }
    return result;
}

bool LocalXmlBackend::todoListToFile(const OpenTodoList::ITodoList *todoList)
{
    if ( todoList ) {
        QDomDocument dom;
        QString fileName = todoList->metaAttribute( TodoListMetaFileName ).toString();
        QFileInfo fi( fileName );
        if ( fi.exists() && fi.isReadable() ) {
            QFile inFile( fileName );
            if ( inFile.open( QIODevice::ReadOnly ) ) {
                QString errorMessage;
                int errorLine, errorColumn;
                if ( !dom.setContent( &inFile, &errorMessage, &errorLine, &errorColumn ) ) {
                    qWarning() << "File" << inFile.fileName() << "is not a valid todo list file";
                    inFile.close();
                    return false;
                }
            } else {
                qWarning() << "Failed to open" << inFile.fileName();
                qWarning() << inFile.errorString();
                return false;
            }
        }

        if ( !listToDom( todoList, dom ) ) {
            qWarning() << "Failed to persist todo list to DOM";
            return false;
        }

        QFile outFile( fileName );
        if ( outFile.open( QIODevice::WriteOnly ) ) {
            outFile.write( dom.toString().toUtf8() );
            outFile.close();
            return true;
        } else {
            qWarning() << "Failed to open" << fileName << "for writing:" << outFile.errorString();
            return false;
        }
    }
    return false;
}

bool LocalXmlBackend::todoToFile(const OpenTodoList::ITodo *todo)
{
    if ( todo ) {
        QDomDocument dom;
        QString fileName = todo->metaAttribute( TodoMetaFileName ).toString();
        QFileInfo fi( fileName );
        if ( fi.exists() && fi.isReadable() ) {
            QFile inFile( fileName );
            if ( inFile.open( QIODevice::ReadOnly ) ) {
                QString errorMessage;
                int errorLine, errorColumn;
                if ( !dom.setContent( &inFile, &errorMessage, &errorLine, &errorColumn ) ) {
                    qWarning() << "File" << inFile.fileName() << "is not a valid todo file";
                    inFile.close();
                    return false;
                }
            } else {
                qWarning() << "Failed to open" << inFile.fileName();
                qWarning() << inFile.errorString();
                return false;
            }
        }

        if ( !todoToDom( todo, dom ) ) {
            qWarning() << "Failed to persist todo to DOM";
            return false;
        }

        QFile outFile( fileName );
        if ( outFile.open( QIODevice::WriteOnly ) ) {
            outFile.write( dom.toString().toUtf8() );
            outFile.close();
            return true;
        } else {
            qWarning() << "Failed to open" << fileName << "for writing:" << outFile.errorString();
            return false;
        }
    }
    return false;
}

bool LocalXmlBackend::listToDom(const OpenTodoList::ITodoList *list, QDomDocument &doc)
{
    QDomElement root = doc.documentElement();
    if ( !root.isElement() ) {
        root = doc.createElement( "todoList" );
        doc.appendChild( root );
    }
    root.setAttribute( "id", list->uuid().toString() );
    root.setAttribute( "name", list->name() );
    root.setAttribute( "lastModificationTime", list->lastModificationTime().toString() );
    return true;
}

bool LocalXmlBackend::domToList(const QDomDocument &doc, OpenTodoList::ITodoList *list)
{
    QDomElement root = doc.documentElement();
    if ( !root.isElement() ) {
        return false;
    }
    if ( root.hasAttribute( "id" ) ) {
        list->setUuid( QUuid( root.attribute( "id", list->uuid().toString() ) ) );
    }
    list->setName( root.attribute( "name", list->name() ) );
    list->setLastModificationTime(
                QDateTime::fromString( root.attribute( list->lastModificationTime().toString() ) ) );
    return true;
}

bool LocalXmlBackend::todoToDom(const OpenTodoList::ITodo *todo, QDomDocument &doc)
{
    QDomElement root = doc.documentElement();
    if ( !root.isElement() ) {
        root = doc.createElement( "todo" );
        doc.appendChild( root );
    }
    root.setAttribute( "id", todo->uuid().toString() );
    root.setAttribute( "title", todo->title() );
    root.setAttribute( "progress", todo->progress() );
    root.setAttribute( "priority", todo->priority() );
    root.setAttribute( "deleted", todo->isDeleted() ? "true" : "false" );
    root.setAttribute( "weight", todo->weight() );
    if ( todo->dueDate().isValid() ) {
        root.setAttribute( "dueDate", todo->dueDate().toString() );
    } else {
        root.removeAttribute( "dueDate" );
    }
    if ( todo->parentTodoUuid().isNull() ) {
        root.removeAttribute( "parent" );
    } else {
        root.setAttribute( "parent", todo->parentTodoUuid().toString() );
    }
    root.setAttribute( "lastModificationDate", todo->lastModificationTime().toString() );
    QDomElement descriptionElement = root.firstChildElement( "description" );
    if ( !descriptionElement.isElement() ) {
        descriptionElement = doc.createElement( "description" );
        root.appendChild( descriptionElement );
    }
    while ( !descriptionElement.firstChild().isNull() ) {
        descriptionElement.removeChild( descriptionElement.firstChild() );
    }
    QDomText descriptionText = doc.createTextNode( todo->description() );
    descriptionElement.appendChild( descriptionText );
    return true;
}

bool LocalXmlBackend::domToTodo(const QDomDocument &doc, OpenTodoList::ITodo *todo)
{
    QDomElement root = doc.documentElement();

    if ( !root.isElement() ) {
        return false;
    }

    if ( root.hasAttribute( "id" ) ) {
        todo->setUuid( QUuid( root.attribute( "id" ) ) );
    }
    todo->setTitle( root.attribute( "title" ) );
    todo->setProgress( qBound( 0, root.attribute( "progress", QString::number(todo->progress()) ).toInt(), 100 ) );
    todo->setPriority( qBound( -1, root.attribute( "priority", QString::number(todo->priority()) ).toInt(), 10 ) );
    todo->setDeleted( root.attribute( "deleted", todo->isDeleted() ? "true" : "false" ) == "true" );
    if ( root.hasAttribute( "dueDate" ) ) {
        todo->setDueDate( QDateTime::fromString( root.attribute( "dueDate" ) ) );
    }
    if ( root.hasAttribute( "parent" ) ) {
        todo->setParentTodoUuid( QUuid( root.attribute( "parent" ) ) );
    }
    todo->setWeight( root.attribute( "weight", QString::number( todo->weight() ) ).toDouble() );
    if ( root.hasAttribute( "lastModificationTime" ) ) {
        todo->setLastModificationTime( QDateTime::fromString( root.attribute( "lastModificationTime" ) ) );
    }
    QDomElement description = root.firstChildElement( "description" );
    if ( description.isElement() ) {
        todo->setDescription( description.text() );
    }
    return true;
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(LocalXmlBackend, LocalXmlBackend)
#endif // QT_VERSION < 0x050000

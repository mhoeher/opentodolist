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

#ifndef LOCALXMLBACKEND_H
#define LOCALXMLBACKEND_H

#include "opentodolistinterfaces.h"

#include <QDomDocument>

class LocalXmlBackend : public QObject, public BackendInterface
{
    Q_OBJECT
    Q_INTERFACES(BackendInterface)
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "net.rpdev.OpenTodoList.Backend/1.0" FILE "LocalXmlBackend.json")
#endif // QT_VERSION >= 0x050000
    
public:
    explicit LocalXmlBackend(QObject *parent = 0);
    virtual ~LocalXmlBackend();

    // BackendInterface interface
    virtual void setDatabase(TodoListDatabase *database);
    virtual void setLocalStorageDirectory(const QString &directory);
    virtual QString id() const;
    virtual QString name() const;
    virtual QString description() const;
    virtual bool start();
    virtual bool stop();
    virtual bool notifyTodoListChanged(const TodoListStruct &list);
    virtual bool notifyTodoChanged(const TodoStruct &todo);

private:

    TodoListDatabase         *m_database;
    QString                   m_localStorageDirectory;

    QStringList locateTodoLists() const;
    QStringList locateTodos( const QString &todoList ) const;

    TodoListStruct todoListFromFile( const QString &fileName );
    TodoStruct todoFromFile(const QString &fileName , double weight);

    static bool todoListToFile( const TodoListStruct &todoList );
    static bool todoToFile( const TodoStruct &todo );

    static bool listToDom( const TodoListStruct &list, QDomDocument &doc );
    static bool domToList( const QDomDocument &doc, TodoListStruct &list );
    static bool todoToDom( const TodoStruct &todo, QDomDocument &doc );
    static bool domToTodo( const QDomDocument &doc, TodoStruct &todo );

    static const QString TodoListConfigFileName;
    static const QString TodoDirectoryName;

    static const QString TodoListMetaFileName;
    static const QString TodoMetaFileName;

};


#endif // LOCALXMLBACKEND_H

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

class LocalXmlBackend : public QObject, public OpenTodoList::IBackend
{
    Q_OBJECT
    Q_INTERFACES(OpenTodoList::IBackend)
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "net.rpdev.OpenTodoList.Backend/1.0" FILE "LocalXmlBackend.json")
#endif // QT_VERSION >= 0x050000
    
public:
    explicit LocalXmlBackend(QObject *parent = 0);
    virtual ~LocalXmlBackend();

    // BackendInterface interface
    void setDatabase(OpenTodoList::IDatabase *database) override;
    void setLocalStorageDirectory(const QString &directory) override;
    QString id() const override;
    QString name() const override;
    QString description() const override;
    bool start() override;
    bool stop() override;

private:

    OpenTodoList::IDatabase         *m_database;
    QString                   m_localStorageDirectory;

    QStringList locateTodoLists() const;
    QStringList locateTodos( const QString &todoList ) const;

    OpenTodoList::ITodoList* todoListFromFile( const QString &fileName );
    OpenTodoList::ITodo* todoFromFile(const QString &fileName , double weight);

    static bool todoListToFile( const OpenTodoList::ITodoList *todoList );
    static bool todoToFile( const OpenTodoList::ITodo *todo );

    static bool listToDom( const OpenTodoList::ITodoList *list, QDomDocument &doc );
    static bool domToList( const QDomDocument &doc, OpenTodoList::ITodoList *list );
    static bool todoToDom( const OpenTodoList::ITodo *todo, QDomDocument &doc );
    static bool domToTodo( const QDomDocument &doc, OpenTodoList::ITodo *todo );

    static const QString TodoListConfigFileName;
    static const QString TodoDirectoryName;

    static const QString TodoListMetaFileName;
    static const QString TodoMetaFileName;

};


#endif // LOCALXMLBACKEND_H

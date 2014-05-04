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

#ifndef ABSTRACTTODOLIST_H
#define ABSTRACTTODOLIST_H

#include "opentodolistinterfaces.h"
#include "todo.h"

#include <QList>
#include <QObject>
#include <QObjectList>
#include <QPointer>
#include <QVariant>
#include <QVariantMap>

class TodoListLibrary;

class TodoList : public QObject
{

    Q_OBJECT

    Q_PROPERTY(bool isNull READ isNull CONSTANT)
    Q_PROPERTY( QString backend READ backend CONSTANT )
    Q_PROPERTY( QString id READ id CONSTANT )
    Q_PROPERTY( QString name READ name WRITE setName NOTIFY nameChanged )
    Q_PROPERTY( TodoListLibrary* library READ library CONSTANT )
    Q_PROPERTY( bool canCreateTodos READ canCreateTodos CONSTANT )
    
public:

    explicit TodoList( QObject *parent = 0 );
    explicit TodoList( const QString &backend,
                       const TodoListStruct &list,
                       TodoListLibrary *library,
                       QObject *parent = 0);
    virtual ~TodoList();

    QString backend() const;
    QString id() const;
    QString name() const;
    void setName( const QString &name );

    TodoListLibrary* library() const;
    
    bool isNull() const;
    void setIsNull(bool isNull);

    bool canCreateTodos() const;

signals:
    
    void nameChanged();
    void changed();
    
public slots:

    void addTodo( const QString &title );
    
protected:
    
private:

    bool                      m_isNull;
    QString                   m_backend;
    TodoListStruct            m_struct;
    QPointer<TodoListLibrary> m_library;
    bool                      m_disablePersisting;
    
private slots:

    void handleTodoListUpdated( const QString &backend, const TodoListStruct &list );
    void handleTodoListRemoved( const QString &backend, const TodoListStruct &list );
    void persist();
    
};

#endif // ABSTRACTTODOLIST_H

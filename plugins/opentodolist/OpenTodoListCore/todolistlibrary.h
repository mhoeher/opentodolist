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

#ifndef TODOLISTLIBRARY_H
#define TODOLISTLIBRARY_H

#include "todolist.h"
#include "pluginsloader.h"
#include "opentodolistinterfaces.h"
#include "backendrunner.h"
#include "todoliststorage.h"

#include <QObject>
#include <QObjectList>
#include <QQmlListProperty>

class TodoListLibrary : public QObject
{
    Q_OBJECT
    Q_PROPERTY( QObject* plugins READ plugins CONSTANT )
    Q_PROPERTY( QQmlListProperty<BackendWrapper> backends READ backends CONSTANT )
    Q_PROPERTY( QString applicationVersion READ applicationVersion CONSTANT )
public:
    
    explicit TodoListLibrary(QObject *parent = 0);
    virtual ~TodoListLibrary();
    
    PluginsLoader *plugins() const;
    TodoListStorage *storage() const;

    bool canAddTodo( const QString &backend, const TodoListStruct &list,
                     const TodoStruct &todo = BackendWrapper::NullTodo );
    void addTodo(const QString &backend, TodoStruct &newTodo,
                  const TodoListStruct &list, const TodoStruct &todo = BackendWrapper::NullTodo );
    QQmlListProperty<BackendWrapper> backends();
    Q_INVOKABLE bool canAddTodoList( const QString &backend );
    Q_INVOKABLE void addTodoList( const QString &backend, const QString &todoListName );

    QString applicationVersion() const;

    // TodoListDatabase wrapper interface
    bool insertTodoList(const BackendInterface *backend,
                        const TodoListStruct &list);
    bool insertTodo(const BackendInterface *backend,
                    const TodoStruct &todo);
    bool deleteTodoList(const BackendInterface *backend,
                        const TodoListStruct &list);
    bool deleteTodo(const BackendInterface *backend,
                    const TodoStruct &todo);

signals:
    
public slots:

    void notifyTodoListChanged( const QString &backend, const TodoListStruct &list );
    void notifyTodoChanged( const QString &backend, const TodoStruct &todo );
    void notifyTodoListDeleted( const QString &backend, const TodoListStruct &list );
    void notifyTodoDeleted( const QString &backend, const TodoStruct &todo );

private:

    PluginsLoader                   *m_plugins;
    BackendRunner                   *m_backendRunner;
    TodoListStorage                 *m_storage;

    static int backendCountFunction( QQmlListProperty<BackendWrapper> *property );
    static BackendWrapper* backendAtFunction(QQmlListProperty<BackendWrapper>* property, int index );

private slots:
    
    void saveSettings();
    void restoreSettings();
    
};

#endif // TODOLISTLIBRARY_H

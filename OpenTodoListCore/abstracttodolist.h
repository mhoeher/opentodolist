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

#include "opentodolistcore_global.h"

#include "abstracttodo.h"
#include "objectmodel.h"
#include "todosortfiltermodel.h"

#include <QList>
#include <QObject>
#include <QObjectList>

class OPENTODOLISTCORESHARED_EXPORT AbstractTodoList : public QObject
{

    Q_OBJECT

    Q_PROPERTY( QString name READ name WRITE setName NOTIFY nameChanged )
    Q_PROPERTY( QString displayName READ displayName NOTIFY nameChanged )
    Q_PROPERTY( QObject* todos READ todos CONSTANT )
    Q_PROPERTY( QObject* entries READ topLevelTodos CONSTANT )
    Q_PROPERTY( QObject* deletedTodos READ deletedTodos CONSTANT )
    Q_PROPERTY( QString type READ type CONSTANT )
    Q_PROPERTY( QString key READ key CONSTANT )
    
public:

    typedef ObjectModel< AbstractTodo > TodoList;

    explicit AbstractTodoList( const QString& key, const QString &type, const QVariant &settings = QVariant(), QObject *parent = 0);
    
    TodoList *todos() const;
    TodoSortFilterModel* topLevelTodos() const;
    TodoSortFilterModel* deletedTodos() const;
    Q_INVOKABLE virtual QObject *addTodo();
    Q_INVOKABLE QObject* addTodo( const QString& title, QObject* parentTodo );
    virtual QVariant settings();

    
    const QString& name() const;
    void setName( const QString& name );

    QString displayName() const;
    
    const QString& type() const {
        return m_type;
    }
    
    const QString& key() const {
        return m_key;
    }

signals:
    
    void nameChanged();
    
    void changed();
    
public slots:
    
protected:
    
    void appendTodo( AbstractTodo* todo );

private:

    TodoList *m_todos;
    TodoSortFilterModel *m_topLevelTodos;
    TodoSortFilterModel *m_deletedTodos;
    QString m_name;
    QString m_type;
    QString m_key;
    QString m_dir;
    
private slots:
    
    void todoParentChanged();
    
};

#endif // ABSTRACTTODOLIST_H

/*
 *  OpenTodoListCore - Core functionality for the OpenTodoList application
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

#ifndef ABSTRACTTODO_H
#define ABSTRACTTODO_H

#include "opentodolistcore_global.h"
#include "todosortfiltermodel.h"

#include <QObject>
#include <qshareddata.h>

class AbstractTodoList;

class OPENTODOLISTCORESHARED_EXPORT AbstractTodo : public QObject
{
    Q_OBJECT
    Q_PROPERTY( QString title READ title WRITE setTitle NOTIFY titleChanged )
    Q_PROPERTY( QString description READ description WRITE setDescription NOTIFY descriptionChanged )
    Q_PROPERTY( int progress READ progress WRITE setProgress NOTIFY progressChanged )
    Q_PROPERTY( int priority READ priority WRITE setPriority NOTIFY priorityChanged )
    Q_PROPERTY( QObject* parentTodo READ parentTodo WRITE setParentTodo NOTIFY parentTodoChanged )
    Q_PROPERTY( bool deleted READ isDeleted WRITE setDeleted NOTIFY deletedChanged )
    Q_PROPERTY( QObject* todoList READ todoList CONSTANT )
    Q_PROPERTY( QObject* subTodos READ subTodos CONSTANT )
    Q_PROPERTY( bool isCompleted READ isCompleted NOTIFY progressChanged() )

public:
    explicit AbstractTodo(AbstractTodoList *parent = 0);
    
    QString title() const;
    void setTitle(const QString &title);

    QString description() const;
    void setDescription(const QString &description);

    int progress() const;
    void setProgress(int progress);

    int priority() const;
    void setPriority(int priority);
    
    AbstractTodo* parentTodo() const;
    void setParentTodo( QObject* parentTodo );

    bool isDeleted() const;
    void setDeleted( bool deleted );

    bool isCompleted() const { return progress() == 100; }
    
    AbstractTodoList* parent() const;
    
    TodoSortFilterModel* subTodos() const {
        return m_subTodosModel;
    }
    
signals:
    
    void titleChanged();
    void descriptionChanged();
    void progressChanged();
    void priorityChanged();
    void parentTodoChanged();
    void deletedChanged();
    
    void changed();
    
public slots:

private:

    QString             m_title;
    QString             m_description;
    int                 m_progress;
    int                 m_priority;
    AbstractTodo        *m_parentTodo;
    bool                m_deleted;
    
    TodoSortFilterModel *m_subTodosModel;
    
    QObject* todoList() const {
        return QObject::parent();
    }
    
private slots:
    
    void childDataChanged();

};

#endif // ABSTRACTTODO_H

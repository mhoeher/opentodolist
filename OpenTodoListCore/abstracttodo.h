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

#ifndef ABSTRACTTODO_H
#define ABSTRACTTODO_H

#include "opentodolistcore_global.h"
#include "todosortfiltermodel.h"

#include <QDate>
#include <QObject>
#include <QUuid>

class AbstractTodoList;

class OPENTODOLISTCORESHARED_EXPORT AbstractTodo : public QObject
{
    Q_OBJECT
    Q_PROPERTY( QUuid id READ id CONSTANT )
    Q_PROPERTY( QString title READ title WRITE setTitle NOTIFY titleChanged )
    Q_PROPERTY( QString description READ description WRITE setDescription NOTIFY descriptionChanged )
    Q_PROPERTY( int progress READ progress WRITE setProgress NOTIFY progressChanged )
    Q_PROPERTY( int priority READ priority WRITE setPriority NOTIFY priorityChanged )
    Q_PROPERTY( QObject* parentTodo READ parentTodo WRITE setParentTodo NOTIFY parentTodoChanged )
    Q_PROPERTY( bool deleted READ isDeleted WRITE setDeleted NOTIFY deletedChanged )
    Q_PROPERTY( QDateTime dueDate READ dueDate WRITE setDueDate NOTIFY dueDateChanged )
    Q_PROPERTY( QObject* todoList READ todoList CONSTANT )
    Q_PROPERTY( QObject* subTodos READ subTodos CONSTANT )
    Q_PROPERTY( bool isCompleted READ isCompleted NOTIFY progressChanged() )
    Q_PROPERTY( int numTodos READ numTodos NOTIFY numTodosChanged )
    Q_PROPERTY( int numOpenTodos READ numOpenTodos NOTIFY numOpenTodosChanged )

public:
    explicit AbstractTodo( QUuid id, AbstractTodoList *parent = 0);
    
    QUuid id() const;

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

    QDateTime dueDate() const;
    void setDueDate( const QDateTime& dateTime );

    bool isCompleted() const { return progress() == 100; }
    
    AbstractTodoList* parent() const;
    
    TodoSortFilterModel* subTodos() const {
        return m_subTodosModel;
    }

    Q_INVOKABLE bool hasSubTodos() const {
        return m_subTodosModel->rowCount() > 0;
    }

    int numTodos() const;
    int numOpenTodos() const;

signals:
    
    void idChanged();
    void titleChanged();
    void descriptionChanged();
    void progressChanged();
    void priorityChanged();
    void parentTodoChanged();
    void deletedChanged();
    void dueDateChanged();
    void hasSubTodosChanged();
    void attachedPropertiesChanged();
    void numTodosChanged();
    void numOpenTodosChanged();
    
    void changed();

    /**
       @brief The todo is about to be disposed

       This signal is emitted by a todo as soon as it is about to be
       disposed. The @p todo parameter carries along the todo itself.
       @param todo
     */
    void aboutToBeDisposed( QObject* todo );
    
public slots:

    void toggleCompleted( int newProgress = -1 );
    void dispose();

protected:

    void setId( QUuid id );

    /**
       @brief Destroies the todo

       Calling this method will cause the todo object being destroyed.
       This method will do nothing. Sub-classes can override it to do cleanup
       (e.g. removing locally allocated resources that were used to
       persist the todo).
     */
    virtual void destroy() {}

private:

    QUuid               m_id;
    QString             m_title;
    QString             m_description;
    int                 m_progress;
    int                 m_priority;
    AbstractTodo        *m_parentTodo;
    bool                m_deleted;
    QDateTime           m_dueDate;

    int                 m_lastProgress;
    
    TodoSortFilterModel *m_subTodosModel;
    
    QObject* todoList() const {
        return QObject::parent();
    }
    
private slots:
    
    void childDataChanged();

};

#endif // ABSTRACTTODO_H

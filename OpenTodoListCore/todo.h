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

#include "opentodolistinterfaces.h"

#include <QObject>


class TodoList;
class TodoListLibrary;

class Todo : public QObject
{
    Q_OBJECT
    Q_PROPERTY( bool isNull READ isNull CONSTANT )
    Q_PROPERTY( QString backend READ backend CONSTANT )
    Q_PROPERTY( QString id READ id CONSTANT )
    Q_PROPERTY( double weight READ weight WRITE setWeight NOTIFY weightChanged )
    Q_PROPERTY( int progress READ progress WRITE setProgress NOTIFY progressChanged )
    Q_PROPERTY( int priority READ priority WRITE setPriority NOTIFY priorityChanged )
    Q_PROPERTY( QString parentTodoId READ parentTodoId CONSTANT )
    Q_PROPERTY( QString todoListId READ todoListId CONSTANT )
    Q_PROPERTY( QDateTime dueDate READ dueDate WRITE setDueDate NOTIFY titleChanged )
    Q_PROPERTY( QString title READ title WRITE setTitle NOTIFY titleChanged )
    Q_PROPERTY( QString description READ description WRITE setDescription NOTIFY descriptionChanged )
    Q_PROPERTY( bool isDeleted READ isDeleted WRITE setDeleted NOTIFY deletedChanged )
    Q_PROPERTY( TodoListLibrary* library READ library CONSTANT )


public:
    explicit Todo( QObject* parent = 0 );
    explicit Todo(const QString &backend,
                   const TodoStruct &todo,
                   TodoListLibrary* library , QObject *parent);
    virtual ~Todo();

    QString backend() const;
    QString id() const;
    double weight() const;
    void setWeight( double weight );
    int progress() const;
    void setProgress( int progress );
    int priority() const;
    void setPriority( int priority );
    QString parentTodoId() const;
    QString todoListId() const;
    QDateTime dueDate() const;
    void setDueDate( const QDateTime &dueDate );
    QString title() const;
    void setTitle( const QString &title );
    QString description() const;
    void setDescription( const QString &description );
    bool isDeleted() const;
    void setDeleted( bool deleted );

    TodoListLibrary *library() const;

    bool isNull() const;

signals:

    void weightChanged();
    void progressChanged();
    void priorityChanged();
    void dueDateChanged();
    void titleChanged();
    void descriptionChanged();
    void deletedChanged();

    void changed();
    
public slots:

protected:

private:

    bool             m_isNull;
    QString          m_backend;
    TodoStruct       m_struct;
    TodoListLibrary* m_library;

};

#endif // ABSTRACTTODO_H

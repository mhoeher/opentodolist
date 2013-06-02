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
    Q_PROPERTY( QObject* todoList READ todoList CONSTANT )
    Q_PROPERTY( QObject* subTodos READ subTodos CONSTANT )

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
    
    void changed();
    
public slots:

private:

    QString             m_title;
    QString             m_description;
    int                 m_progress;
    int                 m_priority;
    AbstractTodo        *m_parentTodo;
    
    TodoSortFilterModel *m_subTodosModel;
    
    QObject* todoList() const {
        return QObject::parent();
    }

};

#endif // ABSTRACTTODO_H

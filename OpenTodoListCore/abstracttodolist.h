#ifndef ABSTRACTTODOLIST_H
#define ABSTRACTTODOLIST_H

#include "opentodolistcore_global.h"

#include <QList>
#include <QObject>
#include <QObjectList>

class AbstractTodo;

class OPENTODOLISTCORESHARED_EXPORT AbstractTodoList : public QObject
{

    Q_OBJECT

public:

    typedef QList< AbstractTodo* > TodoList;

    explicit AbstractTodoList(QObject *parent = 0);
    
    const TodoList &todos() const;
    virtual AbstractTodo *addTodo();

signals:
    
public slots:

private:

    TodoList m_todos;
    
};

#endif // ABSTRACTTODOLIST_H

#include "todolistfactory.h"

TodoListFactory::TodoListFactory(QObject *parent) :
    QObject(parent)
{
}

AbstractTodoList *TodoListFactory::createTodoList(QObject *parent) const
{
    return new AbstractTodoList( parent );
}

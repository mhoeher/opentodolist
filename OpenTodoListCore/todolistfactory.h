#ifndef TODOLISTFACTORY_H
#define TODOLISTFACTORY_H

#include "opentodolistcore_global.h"

#include "abstracttodolist.h"

#include <QObject>

class OPENTODOLISTCORESHARED_EXPORT TodoListFactory : public QObject
{
    Q_OBJECT
public:
    explicit TodoListFactory(QObject *parent = 0);

    virtual AbstractTodoList* createTodoList( QObject *parent = 0 ) const;
    
signals:
    
public slots:
    
};

#endif // TODOLISTFACTORY_H

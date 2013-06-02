#ifndef TODOLISTFACTORY_H
#define TODOLISTFACTORY_H

#include "opentodolistcore_global.h"

#include "abstracttodolist.h"

#include <QObject>

class OPENTODOLISTCORESHARED_EXPORT TodoListFactory : public QObject
{
    Q_OBJECT
    Q_PROPERTY( QString type READ type CONSTANT )
public:
    explicit TodoListFactory(const QString& type, QObject *parent = 0);

    virtual AbstractTodoList* createTodoList( QObject *parent = 0, const QString& key = QString() ) const = 0;
    
    const QString& type() const {
        return m_type;
    }
    
signals:
    
public slots:
    
private:
    
    QString m_type;
    
};

#endif // TODOLISTFACTORY_H

#ifndef OPENTODOLISTINTERFACES_H
#define OPENTODOLISTINTERFACES_H

#include "opentodolistcore_global.h"
#include "todolistfactory.h"

#include <QObject>

class OPENTODOLISTCORESHARED_EXPORT OpenTodoListBackend : public QObject {
    
    Q_OBJECT
    Q_PROPERTY( QString name READ name CONSTANT )
    Q_PROPERTY( QString description READ description CONSTANT )
    Q_PROPERTY( QString type READ type CONSTANT )

public:

    explicit OpenTodoListBackend(QObject* parent = 0) : 
        QObject( parent ), 
        m_name( QString() ), 
        m_description( QString() ) {}
    virtual ~OpenTodoListBackend() {}
    virtual TodoListFactory* factory() = 0;
    
    QString name() const { return m_name; }
    QString description() const { return m_description; }
    QString type() const { return m_type; }
    
protected:
    
    void setName( const QString& name ) { m_name = name; }
    void setDescription( const QString& description ) { m_description = description; }
    void setType( const QString& type ) { m_type = type; }
    
private:
    
    QString m_name;
    QString m_description;
    QString m_type;
    
};

Q_DECLARE_INTERFACE(OpenTodoListBackend, "net.rpdev.OpenTodoList.Backend/1.0")

#endif // OPENTODOLISTINTERFACES_H

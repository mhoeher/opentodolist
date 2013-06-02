#ifndef ABSTRACTTODOLIST_H
#define ABSTRACTTODOLIST_H

#include "opentodolistcore_global.h"

#include "objectmodel.h"
#include "todosortfiltermodel.h"

#include <QList>
#include <QObject>
#include <QObjectList>

class AbstractTodo;

class OPENTODOLISTCORESHARED_EXPORT AbstractTodoList : public QObject
{

    Q_OBJECT

    Q_PROPERTY( QString name READ name WRITE setName NOTIFY nameChanged )
    Q_PROPERTY( QObject* todos READ todos CONSTANT )
    Q_PROPERTY( QObject* entries READ topLevelTodos CONSTANT )
    Q_PROPERTY( QString type READ type CONSTANT )
    Q_PROPERTY( QString key READ key CONSTANT )
    
public:

    typedef ObjectModel< AbstractTodo > TodoList;

    explicit AbstractTodoList( const QString& key, const QString &type, QObject *parent = 0);
    
    TodoList *todos() const;
    TodoSortFilterModel* topLevelTodos() const;
    Q_INVOKABLE virtual QObject *addTodo();
    Q_INVOKABLE QObject* addTodo( const QString& title, QObject* parentTodo );
    
    const QString& name() const;
    void setName( const QString& name );
    
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
    QString m_name;
    QString m_type;
    QString m_key;
    
private slots:
    
    void todoParentChanged();
    
};

#endif // ABSTRACTTODOLIST_H

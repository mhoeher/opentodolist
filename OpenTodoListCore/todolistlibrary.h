#ifndef TODOLISTLIBRARY_H
#define TODOLISTLIBRARY_H

#include "abstracttodolist.h"
#include "pluginsloader.h"
#include "objectmodel.h"
#include "opentodolistinterfaces.h"

#include <QObject>

class TodoListLibrary : public QObject
{
    Q_OBJECT
    Q_PROPERTY( QObject* plugins READ plugins CONSTANT )
    Q_PROPERTY( QObject* todoLists READ todoLists CONSTANT )
public:
    
    typedef ObjectModel< AbstractTodoList > TodoLists;
    
    explicit TodoListLibrary(QObject *parent = 0);
    virtual ~TodoListLibrary();
    
    PluginsLoader *plugins() const;
    TodoLists* todoLists() const;
    
    Q_INVOKABLE bool createTodoList( const QString& name, OpenTodoListBackend* type );

signals:
    
public slots:

private:

    PluginsLoader                   *m_plugins;
    TodoLists                       *m_lists;

    QList<QVariantMap>               m_nonLoadableLists;
    
    OpenTodoListBackend* backendByTypeName( const QString& type );
    
private slots:
    
    void saveSettings();
    void restoreSettings();
    
};

#endif // TODOLISTLIBRARY_H

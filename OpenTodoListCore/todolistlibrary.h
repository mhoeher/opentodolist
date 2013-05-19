#ifndef TODOLISTLIBRARY_H
#define TODOLISTLIBRARY_H

#include "pluginsloader.h"

#include <QObject>

class TodoListLibrary : public QObject
{
    Q_OBJECT
    Q_PROPERTY( QObject* plugins READ plugins )
public:
    explicit TodoListLibrary(QObject *parent = 0);
    
    PluginsLoader *plugins() const;

signals:
    
public slots:

private:

    PluginsLoader *m_plugins;
    
};

#endif // TODOLISTLIBRARY_H

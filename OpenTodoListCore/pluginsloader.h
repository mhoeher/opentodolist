#ifndef PLUGINSLOADER_H
#define PLUGINSLOADER_H

#include "opentodolistinterfaces.h"
#include "objectmodel.h"

#include <QList>
#include <QObject>
#include <QObjectList>

class PluginsLoader : public QObject
{
    Q_OBJECT
    Q_PROPERTY( QObject* backends READ backends CONSTANT )
public:
    
    typedef ObjectModel<OpenTodoListBackend> Backends;
    
    explicit PluginsLoader(QObject *parent = 0);
    
    Backends* backends() const;

signals:
    
public slots:

private:

    Backends* m_backends;
    
};

#endif // PLUGINSLOADER_H

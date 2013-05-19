#ifndef PLUGINSLOADER_H
#define PLUGINSLOADER_H

#include "opentodolistinterfaces.h"

#include <QList>
#include <QObject>

class PluginsLoader : public QObject
{
    Q_OBJECT
public:
    explicit PluginsLoader(QObject *parent = 0);
    
    const QList<OpenTodoListBackend *> &backends() const;

signals:
    
public slots:

private:

    QList< OpenTodoListBackend* > m_backends;
    
};

#endif // PLUGINSLOADER_H

#ifndef OPENTODOLISTSYSTEMINTEGRATIONQMLPLUGIN_H
#define OPENTODOLISTSYSTEMINTEGRATIONQMLPLUGIN_H

#include <QQmlExtensionPlugin>

namespace OpenTodoList {

namespace SystemIntegration {

class Plugin : public QQmlExtensionPlugin
{
    Q_OBJECT
public:
    explicit Plugin(QObject *parent = 0);

    virtual void registerTypes(const char *uri);

signals:

public slots:

};

} /* SystemIntegration */

} /* OpenTodoList */

#endif // OPENTODOLISTSYSTEMINTEGRATIONQMLPLUGIN_H

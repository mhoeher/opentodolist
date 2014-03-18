#ifndef OPENTODOLISTCOREQMLPLUGIN_H
#define OPENTODOLISTCOREQMLPLUGIN_H

#include <QQmlExtensionPlugin>

class OpenTodoListCoreQmlPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA( IID "net.rpdev.opentodolist.core" )

public:
    explicit OpenTodoListCoreQmlPlugin(QObject *parent = 0);

    virtual void registerTypes( const char *uri );

signals:

public slots:

};

#endif // OPENTODOLISTCOREQMLPLUGIN_H

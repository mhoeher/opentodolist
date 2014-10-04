#ifndef OPENTODOLISTCOREQMLPLUGIN_H
#define OPENTODOLISTCOREQMLPLUGIN_H

#include <QQmlExtensionPlugin>

namespace OpenTodoList {

namespace Core {

class Plugin : public QQmlExtensionPlugin
{
    Q_OBJECT

public:
    explicit Plugin(QObject *parent = 0);

    virtual void registerTypes( const char *uri );

signals:

public slots:

};

} /* Core */

} /* OpenTodoList */

#endif // OPENTODOLISTCOREQMLPLUGIN_H

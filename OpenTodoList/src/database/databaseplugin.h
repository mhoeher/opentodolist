#ifndef OPENTODOLISTDATABASEQMLPLUGIN_H
#define OPENTODOLISTDATABASEQMLPLUGIN_H

#include <QQmlExtensionPlugin>

namespace OpenTodoList {

namespace DataBase {

class Plugin : public QQmlExtensionPlugin
{
    Q_OBJECT

public:
    explicit Plugin(QObject *parent = 0);

    virtual void registerTypes( const char *uri );

signals:

public slots:

};

} /* DataBase */

} /* OpenTodoList */

#endif // OPENTODOLISTDATABASEQMLPLUGIN_H

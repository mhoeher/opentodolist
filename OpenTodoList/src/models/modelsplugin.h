#ifndef OPENTODOLISTMODELSQMLPLUGIN_H
#define OPENTODOLISTMODELSQMLPLUGIN_H

#include <QQmlExtensionPlugin>

namespace OpenTodoList {

namespace Models {

class Plugin : public QQmlExtensionPlugin
{
    Q_OBJECT

public:
    explicit Plugin(QObject *parent = 0);

    virtual void registerTypes( const char *uri );

signals:

public slots:

};

} /* Models */

} /* OpenTodoList */

#endif // OPENTODOLISTMODELSQMLPLUGIN_H

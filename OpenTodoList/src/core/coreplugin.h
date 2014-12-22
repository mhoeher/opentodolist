#ifndef OPENTODOLISTCOREQMLPLUGIN_H
#define OPENTODOLISTCOREQMLPLUGIN_H

#include <QQmlExtensionPlugin>

namespace OpenTodoList {

/**
 * @brief Encapsulates core functionality of the application
 *
 * This namespace is used for generic functionality required within the application. This
 * functionality is either in the form of "helper functions" or bridging C++ with QML.
 */
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

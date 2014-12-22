#ifndef OPENTODOLISTDATABASEQMLPLUGIN_H
#define OPENTODOLISTDATABASEQMLPLUGIN_H

#include <QQmlExtensionPlugin>

namespace OpenTodoList {

/**
 * @brief Provides the applications database
 *
 * This namespace contains the classes used to model the applications database.
 */
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

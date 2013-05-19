#ifndef LOCALXMLBACKEND_H
#define LOCALXMLBACKEND_H

#include "opentodolistinterfaces.h"
#include "todolistfactory.h"

class LocalXmlBackend : public QObject, public OpenTodoListBackend
{
    Q_OBJECT
    Q_INTERFACES(OpenTodoListBackend)
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "net.rpdev.OpenTodoList.Backend/1.0" FILE "LocalXmlBackend.json")
#endif // QT_VERSION >= 0x050000
    
public:
    LocalXmlBackend(QObject *parent = 0);

    TodoListFactory *factory();

private:

    TodoListFactory *m_factory;
};

#endif // LOCALXMLBACKEND_H

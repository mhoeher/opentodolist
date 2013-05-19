#ifndef OPENTODOLISTINTERFACES_H
#define OPENTODOLISTINTERFACES_H

#include "opentodolistcore_global.h"
#include "todolistfactory.h"

class OPENTODOLISTCORESHARED_EXPORT OpenTodoListBackend {

public:

    virtual ~OpenTodoListBackend() {}
    virtual TodoListFactory* factory() = 0;
};

Q_DECLARE_INTERFACE(OpenTodoListBackend, "net.rpdev.OpenTodoList.Backend/1.0")

#endif // OPENTODOLISTINTERFACES_H

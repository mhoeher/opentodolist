TARGET = OpenTodoListCore
TEMPLATE = lib
DESTDIR = ../bin

QT += core

HEADERS = \
    abstracttodo.h \
    abstracttodolist.h \
    objectmodel.h \
    opentodolistcore_global.h \
    opentodolistcore.h \
    opentodolistinterfaces.h \
    pluginsloader.h \
    todolistfactory.h \
    todolistlibrary.h \
    todosortfiltermodel.h

SOURCES = \
    abstracttodo.cpp \
    abstracttodolist.cpp \
    objectmodel.cpp \
    opentodolistcore.cpp \
    opentodolistinterfaces.cpp \
    pluginsloader.cpp \
    todolistfactory.cpp \
    todolistlibrary.cpp \
    todosortfiltermodel.cpp

DEFINES +=  OPENTODOLISTCORE_LIBRARY

# Support "make install":
target.path = $$INSTALL_PREFIX/lib
INSTALLS += target

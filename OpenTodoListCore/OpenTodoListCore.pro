TARGET = OpenTodoListCore
TEMPLATE = lib
VERSION = 0.0.0

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

DESTDIR = ../OpenTodoList

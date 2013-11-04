TARGET = OpenTodoListCore
TEMPLATE = lib

# Build without version in file name:
!win32:VERSION = 0.0.0

QT += core

#macx:CONFIG += lib_bundle


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

macx {
    DESTDIR = ../OpenTodoList/OpenTodoList.app/Contents/Frameworks
} else {
    DESTDIR = ../OpenTodoList
}

android {
    x86 {
        target.path = /libs/x86
    } else:armeabi {
        target.path = /libs/armeabi
    } else {
        target.path = /libs/armeabi-v7a
    }
    INSTALLS += target
}

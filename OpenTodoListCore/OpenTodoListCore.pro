TARGET = $$qtLibraryTarget(OpenTodoListCore)
TEMPLATE = lib

QT += core qml sql
CONFIG += plugin

#macx:CONFIG += lib_bundle

HEADERS = \
    objectmodel.h \
    opentodolistinterfaces.h \
    pluginsloader.h \
    todolistfactory.h \
    todolistlibrary.h \
    todosortfiltermodel.h \
    todolist.h \
    todo.h \
    opentodolistcoreqmlplugin.h \
    settings.h \
    backendwrapper.h \
    backendrunner.h \
    todoliststorage.h \
    todoliststoragequery.h

SOURCES = \
    objectmodel.cpp \
    opentodolistinterfaces.cpp \
    pluginsloader.cpp \
    todolistfactory.cpp \
    todolistlibrary.cpp \
    todosortfiltermodel.cpp \
    todolist.cpp \
    todo.cpp \
    opentodolistcoreqmlplugin.cpp \
    settings.cpp \
    backendwrapper.cpp \
    backendrunner.cpp \
    todoliststorage.cpp \
    todoliststoragequery.cpp

DEFINES +=  OPENTODOLISTCORE_LIBRARY

macx {
    DESTDIR = ../OpenTodoList/OpenTodoList.app/Contents/Frameworks/imports/net/rpdev/OpenTodoList/Core
} else {
    DESTDIR = ../OpenTodoList/imports/net/rpdev/OpenTodoList/Core
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

OTHER_FILES += \
    qmldir

QMAKE_POST_LINK += $$QMAKE_COPY $$replace($$list($$quote($$PWD/qmldir) $$DESTDIR), /, $$QMAKE_DIR_SEP)

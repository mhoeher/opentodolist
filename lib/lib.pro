#-------------------------------------------------
#
# Project created by QtCreator 2016-01-25T20:32:47
#
#-------------------------------------------------

include(../config.pri)
setupStaticLib()

QT       += qml quick xml
QT       -= gui

TARGET = opentodolist

SOURCES += \
    application.cpp \
    library.cpp \
    libraryfactory.cpp \
    locallibraryfactory.cpp \
    item.cpp \
    complexitem.cpp \
    toplevelitem.cpp \
    note.cpp \
    image.cpp \
    task.cpp \
    todo.cpp \
    todolist.cpp \
    opentodolistqmlextensionsplugin.cpp \
    documentformatter.cpp

HEADERS += \
    application.h \
    library.h \
    libraryfactory.h \
    locallibraryfactory.h \
    item.h \
    complexitem.h \
    toplevelitem.h \
    note.h \
    image.h \
    task.h \
    todo.h \
    tasklist.h \
    todoslist.h \
    todolist.h \
    toplevelitemlist.h \
    opentodolistqmlextensionsplugin.h \
    documentformatter.h

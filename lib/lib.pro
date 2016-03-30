#-------------------------------------------------
#
# Project created by QtCreator 2016-01-25T20:32:47
#
#-------------------------------------------------

include(../config.pri)
setupStaticLib()

QT       += qml quick xml concurrent
QT       -= gui

TARGET = opentodolist

win32:DESTDIR = $$OUT_PWD

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
    documentformatter.cpp \
    migrator_1_x_to_2_x.cpp \
    fileutils.cpp

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
    documentformatter.h \
    migrator_1_x_to_2_x.h \
    fileutils.h

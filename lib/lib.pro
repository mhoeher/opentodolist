include(../config.pri)

QT       += qml quick xml concurrent
QT       -= gui

TEMPLATE = lib
TARGET = opentodolist
CONFIG += static c++11

win32:DESTDIR = $$OUT_PWD

INCLUDEPATH += datamodel datastorage

SOURCES += \
    application.cpp \
    datamodel/library.cpp \
    datamodel/item.cpp \
    datamodel/complexitem.cpp \
    datamodel/toplevelitem.cpp \
    datamodel/note.cpp \
    datamodel/image.cpp \
    datamodel/task.cpp \
    datamodel/todo.cpp \
    datamodel/todolist.cpp \
    opentodolistqmlextensionsplugin.cpp \
    documentformatter.cpp \
    fileutils.cpp \
    filtermodel.cpp \
    datastorage/itemcontainer.cpp \
    datastorage/libraryloader.cpp

HEADERS += \
    application.h \
    datamodel/library.h \
    datamodel/item.h \
    datamodel/complexitem.h \
    datamodel/toplevelitem.h \
    datamodel/note.h \
    datamodel/image.h \
    datamodel/task.h \
    datamodel/todo.h \
    datamodel/todolist.h \
    opentodolistqmlextensionsplugin.h \
    documentformatter.h \
    fileutils.h \
    filtermodel.h \
    abstractitemmodel.h \
    datastorage/itemcontainer.h \
    datastorage/libraryloader.h

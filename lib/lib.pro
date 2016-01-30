#-------------------------------------------------
#
# Project created by QtCreator 2016-01-25T20:32:47
#
#-------------------------------------------------

include(../config.pri)
setupStaticLib()

QT       += qml xml
QT       -= gui

TARGET = opentodolist

SOURCES += \
    application.cpp \
    library.cpp \
    libraryfactory.cpp \
    locallibraryfactory.cpp

HEADERS += \
    application.h \
    library.h \
    libraryfactory.h \
    locallibraryfactory.h

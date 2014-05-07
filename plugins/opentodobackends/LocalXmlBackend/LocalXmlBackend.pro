TEMPLATE = lib
TARGET = LocalXmlBackend
CONFIG += plugin static

#include(../../../utils.pri)
#pluginConfig(opentodobackends)

QT += xml

INCLUDEPATH += ../../../OpenTodoListCore

SOURCES += \
    localxmlbackend.cpp

HEADERS += \
    localxmlbackend.h

OTHER_FILES += \
    LocalXmlBackend.json

DESTDIR = $$OUT_PWD

TEMPLATE = lib
TARGET = LocalXmlBackend
CONFIG += plugin

include(../../../utils.pri)
pluginConfig(opentodobackends)

QT += xml

SOURCES += \
    localxmlbackend.cpp

HEADERS += \
    localxmlbackend.h

OTHER_FILES += \
    LocalXmlBackend.json


TEMPLATE = lib
TARGET = LocalXmlBackend
CONFIG += plugin

include(../../../utils.pri)
pluginConfig(opentodobackends)

QT += xml

SOURCES += \
    localxmlbackend.cpp \
    localxmltodolistfactory.cpp \
    localxmltodolist.cpp \
    localxmltodo.cpp

HEADERS += \
    localxmltodolistfactory.h \
    localxmltodolist.h \
    localxmltodo.h \
    localxmlbackend.h

OTHER_FILES += \
    LocalXmlBackend.json


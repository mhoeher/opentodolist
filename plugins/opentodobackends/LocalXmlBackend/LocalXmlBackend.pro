TEMPLATE = lib
TARGET = LocalXmlBackend
CONFIG += plugin

DESTDIR = ../../../bin/plugins/opentodobackends

INCLUDEPATH += $$PWD/../../../OpenTodoListCore

LIBS += -L$$DESTDIR/../.. -lOpenTodoListCore

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

target.path = $$INSTALL_PREFIX/lib/OpenTodoList/plugins
INSTALLS += target

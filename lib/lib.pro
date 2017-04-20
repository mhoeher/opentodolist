include(../config.pri)

QT       += qml quick xml concurrent
QT       -= gui

TEMPLATE = lib
TARGET = opentodolist
CONFIG += static c++11 create_prl

win32:DESTDIR = $$OUT_PWD

INCLUDEPATH += datamodel datastorage models

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
    datastorage/itemcontainer.cpp \
    datastorage/libraryloader.cpp \
    models/itemsmodel.cpp \
    models/itemssortfiltermodel.cpp \
    migrators/migrator_2_x_to_3_x.cpp \
    utils/directorywatcher.cpp \
    utils/keystore.cpp

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
    abstractitemmodel.h \
    datastorage/itemcontainer.h \
    datastorage/libraryloader.h \
    models/itemsmodel.h \
    models/itemssortfiltermodel.h \
    migrators/migrator_2_x_to_3_x.h \
    utils/directorywatcher.h \
    utils/keystore.h

config_qtkeychain {
    LIBS += -lqt5keychain
    DEFINES += OTL_USE_SYSTEM_QT5KEYCHAIN
    DBUS_INTERFACES += $$PWD/../3rdparty/qtkeychain/org.kde.KWallet.xml
} else {
    include(../3rdparty/qtkeychain/qt5keychain.pri)
}

include(../3rdparty/simplecrypt.pri)

include(../config.pri)
setupApplication()

TARGET = OpenTodoList

QT += qml quick widgets svg xml concurrent

SOURCES += main.cpp

RESOURCES += qml.qrc \
    res.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH = $$PWD

# Default rules for deployment.
include(deployment.pri)

# Android: Set app name and point to Android files to be used:
ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
ANDROID_PACKAGE = net.rpdev.opentodolist
ANDROID_APP_NAME = Open Todo List

# Icons for various platforms
mac:ICON = red/OpenTodoList.icns
win32:RC_FILE = OpenTodoList.rc

TEMPLATE = app
TARGET = OpenTodoList

include(../config.pri)
include(../lib/lib.pri)

QT += qml quick xml concurrent

SOURCES += main.cpp

RESOURCES += qml.qrc \
    res.qrc

!ios&!android {
    DEFINES += OTL_USE_SINGLE_APPLICATION
    HEADERS += \
        ../3rdparty/qtsingleapplication/src/qtsingleguiapplication.h \
        ../3rdparty/qtsingleapplication/src/qtlocalpeer.h
    SOURCES += \
        ../3rdparty/qtsingleapplication/src/qtsingleguiapplication.cpp \
        ../3rdparty/qtsingleapplication/src/qtlocalpeer.cpp
    INCLUDEPATH += ../3rdparty/qtsingleapplication/src
    DEPENDPATH += ../3rdparty/qtsingleapplication/src
}

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH = $$PWD

# Android: Set app name and point to Android files to be used:
ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
ANDROID_PACKAGE = net.rpdev.opentodolist
ANDROID_APP_NAME = Open Todo List

# Icons for various platforms
mac:ICON = res/OpenTodoList.icns
win32:RC_FILE = OpenTodoList.rc

OTHER_FILES += \
    android/AndroidManifest.xml


target.path = $$INSTALL_PREFIX$$INSTALL_SUFFIX_BIN
INSTALLS += target

DISTFILES += \
    android/AndroidManifest.xml \
    android/gradlew \
    android/android.iml \
    android/build.gradle \
    android/gradlew.bat \
    android/gradle.properties \
    android/local.properties

contains(ANDROID_TARGET_ARCH,armeabi-v7a) {
    ANDROID_EXTRA_LIBS = \
        $$PWD/../pre-build/android/openssl/arm-linux-androideabi-4.9-api-18/libcrypto.so \
        $$PWD/../pre-build/android/openssl/arm-linux-androideabi-4.9-api-18/libssl.so
}

contains(ANDROID_TARGET_ARCH,x86) {
    ANDROID_EXTRA_LIBS = \
        $$PWD/../pre-build/android/openssl/x86-4.9-api-18/libcrypto.so \
        $$PWD/../pre-build/android/openssl/x86-4.9-api-18/libssl.so
}

TEMPLATE = app
TARGET = OpenTodoList

include(../config.pri)
include(../lib/lib.pri)

QT += qml quick xml concurrent widgets
CONFIG += c++11

SOURCES += main.cpp

RESOURCES += qml.qrc \
    res.qrc \
    translations.qrc

!ios&!android {
    DEFINES += OTL_USE_SINGLE_APPLICATION
    HEADERS += \
        ../3rdparty/qtsingleapplication/src/qtsingleapplication.h \
        ../3rdparty/qtsingleapplication/src/qtlocalpeer.h
    SOURCES += \
        ../3rdparty/qtsingleapplication/src/qtsingleapplication.cpp \
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
    android/local.properties \
    OpenTodoList/UI/SyncIndicatorBar.qml \
    OpenTodoList/UI/LibraryPageMenu.qml

contains(ANDROID_TARGET_ARCH,armeabi-v7a) {
    OPENSSL_PREBUILD_DIR = \
        $$PWD/../pre-build/android/openssl/arm-linux-androideabi-4.9-api-18
}
contains(ANDROID_TARGET_ARCH,x86) {
    OPENSSL_PREBUILD_DIR = \
        $$PWD/../pre-build/android/openssl/x86-4.9-api-18/
}
android {
    exists($$OPENSSL_PREBUILD_DIR/libcrypto.so):
    exists($$OPENSSL_PREBUILD_DIR/libssl.so) {
        ANDROID_EXTRA_LIBS = \
            $$OPENSSL_PREBUILD_DIR/libcrypto.so \
            $$OPENSSL_PREBUILD_DIR/libssl.so
        message("OpenSSL libraries for Android found - your build will support HTTPS")
    } else {
        warning("No prebuilt OpenSSL libraries found for Android.")
        warning("Your build will not support HTTPS connections!")
        warning("To fix this, build OpenSSL libraries and put them into")
        warning("$$OPENSSL_PREBUILD_DIR")
    }
}

with_appimage_extras {
    # If we build an AppImage, explicitly link against libssl.so and
    # libcrypto.so so we bundle them. This is required because on more
    # recent distributions a (binary) incompatible OpenSSL is available
    # which we cannot really use and hence no HTTPS is working.
    LIBS += -lcrypto -lssl
}


# Add QML/JS files when running lupdate:
lupdate_only {
    SOURCES += $$files(*.qml,true) $$files(*.js,true)
}

TRANSLATIONS += \
    translations/opentodolist_en.ts \
    translations/opentodolist_de.ts

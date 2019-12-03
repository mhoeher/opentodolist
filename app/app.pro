TEMPLATE = app
TARGET = OpenTodoList

include(../config.pri)
include(../lib/lib.pri)

QT += qml quick xml concurrent widgets

# Use Qt Quick Compiler for release builds:
CONFIG(release, debug|release) {
    # Note: This option must come before the embed_translations option,
    #       otherwise, running qmake yields errors.
    #       See https://bugreports.qt.io/browse/QTBUG-79672
    CONFIG += qtquickcompiler
    QTQUICK_COMPILER_SKIPPED_RESOURCES += res.qrc
}

CONFIG += c++11 lrelease embed_translations

SOURCES += main.cpp

RESOURCES += \
    qml.qrc \
    res.qrc

!ios&!android {
    include(../3rdparty/SingleApplication/singleapplication.pri)
    DEFINES += OTL_USE_SINGLE_APPLICATION QAPPLICATION_CLASS=QGuiApplication
}

# Set VERSION variable, it is required for apps on some platforms,
# such as the iOS simulator:
system(git describe --tags) {
    VERSION = $$system(git describe --tags)
} else {
    VERSION = 3.0.0-unknown
}

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH = $$PWD

# Android: Set app name and point to Android files to be used:
ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
ANDROID_PACKAGE = net.rpdev.opentodolist
ANDROID_APP_NAME = OpenTodoList

# Icons for various platforms
mac:ICON = res/OpenTodoList.icns
win32:RC_FILE = OpenTodoList.rc

target.path = $$INSTALL_PREFIX$$INSTALL_SUFFIX_BIN
INSTALLS += target

OTHER_FILES += $$files(android/*,true)


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


# Add QML/JS files when running lupdate:
lupdate_only {
    SOURCES += $$files(*.qml,true) $$files(*.js,true)
}


TRANSLATIONS += \
    translations/opentodolist_en.ts \
    translations/opentodolist_de.ts \
    translations/opentodolist_fr.ts

QM_FILES_RESOURCE_PREFIX = :/translations/

# iOS specific setup
ios {
    QMAKE_TARGET_BUNDLE_PREFIX = net.rpdev
    QMAKE_INFO_PLIST = ios/Info.plist

    ios_icon.files = $$files($$PWD/ios/AppIcon/AppIcon*.png)
    QMAKE_BUNDLE_DATA += ios_icon
    OTHER_FILES += $$files($$PWD/ios/AppIcon/AppIcon*.png)
}

DISTFILES += \
    ios/exportOptions.plist


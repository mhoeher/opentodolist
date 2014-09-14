TARGET = OpenTodoList
TEMPLATE = app

include(../config.pri)
setupApplication()

# On Android, prefer QRC deployment
!android {

# Add basic folders for deploymend
qml_main.source = qml/OpenTodoList
qml_main.files = $$files($$qml_main.source/*)
qml_main.target = $${assets_prefix}OpenTodoList/qml

qml_components.source = qml/net/rpdev/OpenTodoList/Components
qml_components.target = $${assets_prefix}OpenTodoList/qml/net/rpdev/OpenTodoList

qml_theme.source = qml/net/rpdev/OpenTodoList/Theme
qml_theme.target = $${assets_prefix}OpenTodoList/qml/net/rpdev/OpenTodoList

qml_views.source = qml/net/rpdev/OpenTodoList/Views
qml_views.target = $${assets_prefix}OpenTodoList/qml/net/rpdev/OpenTodoList

DEPLOYMENTFOLDERS = qml_main qml_components qml_theme qml_views

# Install Application Icons on UNIX:
unix {
    unix_icons.source = icons
    unix_icons.target = $${assets_prefix}OpenTodoList
    DEPLOYMENTFOLDERS += unix_icons
}

}

qtcAddDeployment()

# Additional import path used to resolve QML modules in Creator's code model
QML_IMPORT_PATH = $$PWD/../OpenTodoListCore/imports $$PWD/qml

# Icons for various platforms
mac:ICON = OpenTodoList.icns
win32:RC_FILE = OpenTodoList.rc

# Qt dependencies
QT += network

# Handling of status notifiers / system tray icons
qtHaveModule(KNotifications) {
    QT += KNotifications
    DEFINES += HAS_KNOTIFICATIONS HAS_QT_WIDGETS
} else {
    win32|macx|unix {
        QT += widgets
        DEFINES += HAS_QSYSTEM_TRAY_ICON HAS_QT_WIDGETS
    }
}

# Android special handling: Needs to depend also on all used Qt modules
QT += core qml quick sql xml

HEADERS += \
    applicationinstance.h \
    statusnotifiericon.h \
    commandhandler.h


SOURCES += \
    applicationinstance.cpp \
    main.cpp \
    statusnotifiericon.cpp \
    commandhandler.cpp


# Android specific:
ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
ANDROID_PACKAGE = net.rpdev.opentodolist
ANDROID_APP_NAME = Open Todo List
OTHER_FILES += \
    android/AndroidManifest.xml \
    ../bin/linux-deploy-qt.pl \
    ../bin/mk-installer.pl \
    ../templates/installer/config/config.xml \
    ../templates/installer/packages/net.rpdev.OpenTodoList/meta/package.xml \
    ../templates/installer/packages/net.rpdev.OpenTodoList/meta/script.js

android:RESOURCES += OpenTodoList.qrc


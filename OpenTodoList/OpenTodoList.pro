TARGET = OpenTodoList
TEMPLATE = app

include(../config.pri)
setupApplication()

# Install Application Icons on UNIX:
!android {
unix {
    unix_icons.source = icons
    unix_icons.target = $${assets_prefix}OpenTodoList
    DEPLOYMENTFOLDERS += unix_icons
}
}

qtcAddDeployment()

# Additional import path used to resolve QML modules in Creator's code model
QML_IMPORT_PATH = $$PWD/qml

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

INCLUDEPATH += \
    inc \
    src

HEADERS += \
    inc/core/opentodolistinterfaces.h \
    src/listutils.h \
    src/pluginsloader.h \
    src/core/documentformatter.h \
    src/core/settings.h \
    src/database/backendwrapper.h \
    src/datamodel/todo.h \
    src/datamodel/todolist.h \
    src/datamodel/todolistlibrary.h \
    src/models/todolistmodel.h \
    src/models/todomodel.h \
    src/systemintegration/shortcut.h \
    src/systemintegration/applicationinstance.h \
    src/systemintegration/commandhandler.h \
    src/systemintegration/statusnotifiericon.h \
    src/core/coreplugin.h \
    src/database/databaseplugin.h \
    src/datamodel/datamodelplugin.h \
    src/models/modelsplugin.h \
    src/systemintegration/systemintegrationplugin.h \
    src/database/databaseworker.h \
    src/database/database.h \
    src/database/storagequery.h \
    src/datamodel/account.h \

SOURCES += \
    src/main.cpp \
    src/core/documentformatter.cpp \
    src/core/settings.cpp \
    src/database/backendwrapper.cpp \
    src/datamodel/todo.cpp \
    src/datamodel/todolist.cpp \
    src/datamodel/todolistlibrary.cpp \
    src/models/todolistmodel.cpp \
    src/models/todomodel.cpp \
    src/systemintegration/shortcut.cpp \
    src/systemintegration/applicationinstance.cpp \
    src/systemintegration/commandhandler.cpp \
    src/systemintegration/statusnotifiericon.cpp \
    src/core/coreplugin.cpp \
    src/database/databaseplugin.cpp \
    src/datamodel/datamodelplugin.cpp \
    src/models/modelsplugin.cpp \
    src/systemintegration/systemintegrationplugin.cpp \
    src/database/databaseworker.cpp \
    src/database/database.cpp \
    src/database/storagequery.cpp \
    src/datamodel/account.cpp \

RESOURCES += OpenTodoList.qrc

# To let QML files appear in project browser in Qt Creator
OTHER_FILES += \
    qml/net/rpdev/OpenTodoList/Components/*.* \
    qml/net/rpdev/OpenTodoList/Theme/*.* \
    qml/net/rpdev/OpenTodoList/Views/*.* \
    qml/OpenTodoList/*.* \
    ../bin/mk-qrc.pl


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

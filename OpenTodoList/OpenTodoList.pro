TARGET = OpenTodoList
TEMPLATE = app

include(../config.pri)
setupApplication()

# Install Application Icons on UNIX:
!android|!macx|!ios {
    icons.source = icons
    icons.target = $${assets_prefix}OpenTodoList
    DEPLOYMENTFOLDERS += icons
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

DEPENDPATH += \
  inc/core \
  src \
  src/core \
  src/database \
  src/database/queries \
  src/database/queries/private \
  src/datamodel \
  src/models \
  src/models/private \
  src/systemintegration

HEADERS += \
    inc/core/opentodolistinterfaces.h \
    src/pluginsloader.h \
    src/core/documentformatter.h \
    src/core/settings.h \
    src/database/backendwrapper.h \
    src/datamodel/todo.h \
    src/datamodel/todolist.h \
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
    src/datamodel/task.h \
    src/datamodel/backend.h \
    src/database/queries/insertbackend.h \
    src/database/queries/insertaccount.h \
    src/database/queries/readbackend.h \
    src/database/queries/readaccount.h \
    src/systemintegration/application.h \
    src/models/backendmodel.h \
    src/models/accountmodel.h \
    src/database/queries/inserttodolist.h \
    src/database/queries/readtodolist.h \
    src/database/queries/inserttodo.h \
    src/database/queries/readtodo.h \
    src/database/queries/inserttask.h \
    src/database/queries/readtask.h \
    src/models/taskmodel.h \
    src/database/queries/private/insertobject.h \
    src/database/queries/private/readobject.h \
    src/datamodel/objectinfo.h \
    src/database/queries/deleteaccount.h \
    src/database/queries/private/deleteobject.h \
    src/database/queries/deletetodolist.h \
    src/database/queries/deletetodo.h \
    src/database/queries/deletetask.h \
    src/database/queries/private/saveobject.h \
    src/database/queries/saveaccount.h \
    src/database/queries/savetodolist.h \
    src/database/queries/savetodo.h \
    src/database/queries/savetask.h \
    src/database/queries/private/disposeobject.h \
    src/database/queries/disposeaccount.h \
    src/database/queries/disposetodolist.h \
    src/database/queries/disposetodo.h \
    src/database/queries/disposetask.h \
    src/database/databaseconnection.h \
    src/models/private/objectmodel.h

SOURCES += \
    src/main.cpp \
    src/core/documentformatter.cpp \
    src/core/settings.cpp \
    src/database/backendwrapper.cpp \
    src/datamodel/todo.cpp \
    src/datamodel/todolist.cpp \
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
    src/datamodel/task.cpp \
    src/datamodel/backend.cpp \
    src/database/queries/insertbackend.cpp \
    src/database/queries/insertaccount.cpp \
    src/database/queries/readbackend.cpp \
    src/database/queries/readaccount.cpp \
    src/systemintegration/application.cpp \
    src/models/backendmodel.cpp \
    src/models/accountmodel.cpp \
    src/database/queries/inserttodolist.cpp \
    src/database/queries/readtodolist.cpp \
    src/database/queries/inserttodo.cpp \
    src/database/queries/readtodo.cpp \
    src/database/queries/inserttask.cpp \
    src/database/queries/readtask.cpp \
    src/models/taskmodel.cpp \
    src/database/queries/deleteaccount.cpp \
    src/database/queries/deletetodolist.cpp \
    src/database/queries/deletetodo.cpp \
    src/database/queries/deletetask.cpp \
    src/database/queries/saveaccount.cpp \
    src/database/queries/savetodolist.cpp \
    src/database/queries/savetodo.cpp \
    src/database/queries/savetask.cpp \
    src/database/queries/disposeaccount.cpp \
    src/database/queries/disposetodolist.cpp \
    src/database/queries/disposetodo.cpp \
    src/database/queries/disposetask.cpp \
    src/database/databaseconnection.cpp \
    src/models/private/objectmodel.cpp

RESOURCES += OpenTodoList.qrc

# To let QML files appear in project browser in Qt Creator
OTHER_FILES += \
    ../bin/mk-qrc.pl \
    OpenTodoList.dox \
    doc/databaseproto.dox \
    doc/doxygen-opentodolist.css \
    ../Doxyfile


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

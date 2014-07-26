include(../config.pri)

# Add basic folders for deploymend
qml_main.source = qml/OpenTodoList
qml_main.target = ../share/OpenTodoList/qml

qml_components.source = qml/net/rpdev/OpenTodoList/Components
qml_components.target = ../share/OpenTodoList/qml/net/rpdev/OpenTodoList

qml_theme.source = qml/net/rpdev/OpenTodoList/Theme
qml_theme.target = ../share/OpenTodoList/qml/net/rpdev/OpenTodoList

qml_views.source = qml/net/rpdev/OpenTodoList/Views
qml_views.target = ../share/OpenTodoList/qml/net/rpdev/OpenTodoList

DEPLOYMENTFOLDERS = qml_main qml_components qml_theme qml_views

# Install Application Icons on UNIX:
unix {
    unix_icons.source = icons
    unix_icons.target = ../share/OpenTodoList
    DEPLOYMENTFOLDERS += unix_icons
}

# Additional import path used to resolve QML modules in Creator's code model
QML_IMPORT_PATH = $$PWD/../OpenTodoListCore/imports $$PWD/qml

# Icons for various platforms
mac:ICON = OpenTodoList.icns
win32:RC_FILE = OpenTodoList.rc

# Qt dependencies
QT += sql xml

# The .cpp file which was generated for your project. Feel free to hack it.
SOURCES += main.cpp

# Link against libraries/plugins:
LIBS += -L../OpenTodoListCore -lOpenTodoListCore \
        -L../plugins/opentodobackends/LocalXmlBackend -lLocalXmlBackend

# Installation path
target.path = /OpenTodoList/bin

# Please do not modify the following two lines. Required for deployment.
include(qtquick2applicationviewer/qtquick2applicationviewer.pri)
qtcAddDeployment()

# Android specific:
ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
ANDROID_PACKAGE = net.rpdev.opentodolist
ANDROID_APP_NAME = Open Todo List
OTHER_FILES += \
    android/AndroidManifest.xml \
    ../templates/installer/config/config.xml \
    ../templates/installer/packages/net.rpdev.OpenTodoList/meta/package.xml \
    ../bin/mk-installer.pl

android:RESOURCES += OpenTodoList.qrc

!macx:DESTDIR = ../bin

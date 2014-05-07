# Add more folders to ship with the application, here
qml_main.source = qml/OpenTodoList
qml_main.target = qml

qml_components.source = qml/net/rpdev/OpenTodoList/Components
qml_components.target = qml/net/rpdev/OpenTodoList

qml_theme.source = qml/net/rpdev/OpenTodoList/Theme
qml_theme.target = qml/net/rpdev/OpenTodoList

qml_views.source = qml/net/rpdev/OpenTodoList/Views
qml_views.target = qml/net/rpdev/OpenTodoList

# Workaround: This yields a better view in Qt Creator ;)
debug:DEPLOYMENTFOLDERS = qml_main qml_components qml_theme qml_views

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
# target.path =

# Please do not modify the following two lines. Required for deployment.
include(qtquick2applicationviewer/qtquick2applicationviewer.pri)
qtcAddDeployment()

# Android specific:
ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
ANDROID_PACKAGE = net.rpdev.opentodolist
ANDROID_APP_NAME = Open Todo List
OTHER_FILES += \
    android/AndroidManifest.xml

RESOURCES += OpenTodoList.qrc

win32:DESTDIR = $$OUT_PWD

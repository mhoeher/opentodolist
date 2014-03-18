TARGET = OpenTodoList
TEMPLATE = app

INCLUDEPATH += ../OpenTodoListCore

DESTDIR = $$OUT_PWD

mac {
    #LIBS += -L$$DESTDIR/OpenTodoList.app/Contents/Frameworks -lOpenTodoListCore
    #QMAKE_POST_LINK = install_name_tool -change libOpenTodoListCore.0.dylib \
    #                  @executable_path/../Frameworks/libOpenTodoListCore.0.dylib \
    #                  $$DESTDIR/OpenTodoList.app/Contents/MacOs/OpenTodoList
    ICON = OpenTodoList.icns

} else {
    #LIBS += -L$$DESTDIR -lOpenTodoListCore
}

# Seems that does not help... :(
# The assets seem to remain in the *.apk and thus cannot
# be loaded directly.
#android {
#    plugins_backends_folder.source = $$OUT_PWD/plugins/
#    plugins_backends_folder.path = .
#    DEPLOYMENTFOLDERS += plugins_backends_folder
#}

# FIXME: Do not statically link plugins as soon as this works
android {
    QT += xml
    LIBS += -L$$OUT_PWD/plugins/opentodobackends -lLocalXmlBackend
}

QT += qml quick widgets

# TODO: Remove me!
CONFIG+=qml_debug

HEADERS += \
    applicationviewer.h \
    imageprovider.h

SOURCES += \
    main.cpp \
    applicationviewer.cpp \
    imageprovider.cpp

RESOURCES += \
    resources.qrc

win32:RC_FILE = OpenTodoList.rc

include(files.pri)



# Additional import path used to resolve QML modules in Creator's code model
QML_IMPORT_PATH = imports $$OUT_PWD/imports

# If your application uses the Qt Mobility libraries, uncomment the following
# lines and add the respective components to the MOBILITY variable.
# CONFIG += mobility
# MOBILITY +=

# Please do not modify the following two lines. Required for deployment.
include(qtquick2applicationviewer/qtquick2applicationviewer.pri)
qtcAddDeployment()

# Extra Target for updating the resources.qrc file:
update_qrc.target = update_qrc
update_qrc.commands = \
    perl $$PWD/../bin/mk-qrc.pl -d $$PWD/qml/ -o $$PWD/resources.qrc -b $$PWD/qml
QMAKE_EXTRA_TARGETS += update_qrc

update_pri.target = update_pri
update_pri.commands = \
    perl $$PWD/../bin/mk-pri.pl -d $$PWD/qml -o $$PWD/files.pri -b $$PWD/qml
QMAKE_EXTRA_TARGETS += update_pri

update.depends = update_qrc update_pri
update.target = update
QMAKE_EXTRA_TARGETS += update

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
ANDROID_PACKAGE = net.rpdev.opentodolist
ANDROID_APP_NAME = Open Todo List
ANDROID_EXTRA_LIBS = $$OUT_PWD/libOpenTodoListCore.so

OTHER_FILES += \
    android/AndroidManifest.xml

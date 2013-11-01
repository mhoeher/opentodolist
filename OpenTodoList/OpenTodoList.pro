TARGET = OpenTodoList
TEMPLATE = app

INCLUDEPATH += ../OpenTodoListCore

DESTDIR = .

mac {
    LIBS += -L$$DESTDIR/OpenTodoList.app/Contents/Frameworks -lOpenTodoListCore
    QMAKE_POST_LINK = install_name_tool -change libOpenTodoListCore.0.dylib \
                      @executable_path/../Frameworks/libOpenTodoListCore.0.dylib \
                      $$DESTDIR/OpenTodoList.app/Contents/MacOs/OpenTodoList
    ICON = OpenTodoList.icns

} else {
    LIBS += -L$$DESTDIR -lOpenTodoListCore
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
    imageprovider.h \
    settings.h

SOURCES += \
    main.cpp \
    applicationviewer.cpp \
    imageprovider.cpp \
    settings.cpp

RESOURCES += \
    resources-js.qrc \
    resources-res.qrc \
    resources-style-neutral.qrc

include(files-js.pri)
include(files-res.pri)
include(files-style-neutral.pri)

!android {
    RESOURCES += resources-style-widget.qrc
    include(files-style-widget.pri)
}



# Additional import path used to resolve QML modules in Creator's code model
QML_IMPORT_PATH =

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
    perl $$PWD/../bin/mk-qrc.pl -d $$PWD/qml/OpenTodoList/js -o $$PWD/resources-js.qrc -b $$PWD/qml && \
    perl $$PWD/../bin/mk-qrc.pl -d $$PWD/qml/OpenTodoList/res -o $$PWD/resources-res.qrc -b $$PWD/qml && \
    perl $$PWD/../bin/mk-qrc.pl -d $$PWD/qml/OpenTodoList/styles/neutral -o $$PWD/resources-style-neutral.qrc -b $$PWD/qml && \
    perl $$PWD/../bin/mk-qrc.pl -d $$PWD/qml/OpenTodoList/styles/widget -o $$PWD/resources-style-widget.qrc -b $$PWD/qml
QMAKE_EXTRA_TARGETS += update_qrc

update_pri.target = update_pri
update_pri.commands = \
    perl $$PWD/../bin/mk-pri.pl -d $$PWD/qml/OpenTodoList/js -o $$PWD/files-js.pri -b $$PWD/qml && \
    perl $$PWD/../bin/mk-pri.pl -d $$PWD/qml/OpenTodoList/res -o $$PWD/files-res.pri -b $$PWD/qml && \
    perl $$PWD/../bin/mk-pri.pl -d $$PWD/qml/OpenTodoList/styles/neutral -o $$PWD/files-style-neutral.pri -b $$PWD/qml && \
    perl $$PWD/../bin/mk-pri.pl -d $$PWD/qml/OpenTodoList/styles/widget -o $$PWD/files-style-widget.pri -b $$PWD/qml
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

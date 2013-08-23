TARGET = OpenTodoList
TEMPLATE = app

INCLUDEPATH += ../OpenTodoListCore

# Note: Plugins need to come BEFORE the core libraries in order for the
#       linker to find all symbols correctly.
android:LIBS += -L../plugins/opentodobackends/LocalXmlBackend -lLocalXmlBackend
android:QT += xml

LIBS += -L../OpenTodoListCore -lOpenTodoListCore


QT += quick

HEADERS += \
    applicationviewer.h \
    imageprovider.h

SOURCES += \
    main.cpp \
    applicationviewer.cpp \
    imageprovider.cpp

RESOURCES += \
    resources.qrc

include(files.pri)

# Additional import path used to resolve QML modules in Creator's code model
QML_IMPORT_PATH =

# If your application uses the Qt Mobility libraries, uncomment the following
# lines and add the respective components to the MOBILITY variable.
# CONFIG += mobility
# MOBILITY +=

# Please do not modify the following two lines. Required for deployment.
include(qtquick2applicationviewer/qtquick2applicationviewer.pri)
#qtcAddDeployment()

# Extra Target for updating the resources.qrc file:
update_qrc.target = update_qrc
update_qrc.commands = perl $$PWD/../bin/mk-qrc.pl -d $$PWD/qml -o $$PWD/resources.qrc
QMAKE_EXTRA_TARGETS += update_qrc

update_pri.target = update_pri
update_pri.commands = perl $$PWD/../bin/mk-pri.pl -d $$PWD/qml -o $$PWD/files.pri
QMAKE_EXTRA_TARGETS += update_pri

update.depends = update_qrc update_pri
update.target = update
QMAKE_EXTRA_TARGETS += update

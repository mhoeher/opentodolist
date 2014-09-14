include(../../../config.pri)
setupPlugin(OpenTodoListCore,opentodolist)

# Add basic folders for deploymend
core_main.source = qml/net/rpdev/OpenTodoList/Core
core_main.target = $${assets_prefix}OpenTodoList/qml/net/rpdev/OpenTodoList

DEPLOYMENTFOLDERS += core_main

qtcAddDeployment()

QT += core qml quick sql

QMAKE_MOC_OPTIONS += -Muri=net.rpdev.OpenTodoList.Core

HEADERS = \
    opentodolistinterfaces.h \
    pluginsloader.h \
    todolistlibrary.h \
    todolist.h \
    todo.h \
    opentodolistcoreqmlplugin.h \
    settings.h \
    backendwrapper.h \
    backendrunner.h \
    todoliststorage.h \
    todoliststoragequery.h \
    todolistmodel.h \
    todomodel.h \
    documentformatter.h \
    listutils.h

SOURCES = \
    pluginsloader.cpp \
    todolistlibrary.cpp \
    todolist.cpp \
    todo.cpp \
    opentodolistcoreqmlplugin.cpp \
    settings.cpp \
    backendwrapper.cpp \
    backendrunner.cpp \
    todoliststorage.cpp \
    todoliststoragequery.cpp \
    todolistmodel.cpp \
    todomodel.cpp \
    documentformatter.cpp

DEFINES +=  OPENTODOLISTCORE_LIBRARY

#macx {
#    QML_MOD_PATH = ../OpenTodoList/OpenTodoList.app/Contents/Frameworks/imports/net/rpdev/OpenTodoList/Core
#} else {
#    QML_MOD_PATH = ../OpenTodoList/imports/net/rpdev/OpenTodoList/Core
#}

#android {
#    x86 {
#        target.path = /libs/x86
#    } else:armeabi {
#        target.path = /libs/armeabi
#    } else {
#        target.path = /libs/armeabi-v7a
#    }
#    INSTALLS += target
#}

#OTHER_FILES += \
#    qmldir

#QMAKE_POST_LINK += $$QMAKE_COPY $$replace($$list($$quote($$PWD/qmldir) $$OUT_PWD/../OpenTodoList/imports/net/rpdev/OpenTodoList/Core), /, $$QMAKE_DIR_SEP)

OTHER_FILES += \
    OpenTodoListCorePlugin.json qml/net/rpdev/OpenTodoList/Core/qmldir

RESOURCES += \
    OpenTodoListCore.qrc

# The application version
OPENTODOLIST_VERSION=2.0.2

# Pass in values via defines
DEFINES += VERSION=\\\"$$OPENTODOLIST_VERSION\\\"

QMAKE_TARGET_COMPANY = "RPdev"
QMAKE_TARGET_PRODUCT = "OpenTodoList"
QMAKE_TARGET_DESCRIPTION = "A todo and task management application"
QMAKE_TARGET_COPYRIGHT = "(c) RPdev 2014-2015"

CONFIG(release, debug|release) {
    DEFINES += OPENTODOLIST_RELEASE
}

CONFIG(debug, debug|release) {
    DEFINES += OPENTODOLIST_DEBUG
}

defineTest(setupApplication) {
    TEMPLATE = app
    CONFIG += c++11
    INCLUDEPATH += $$PWD/../lib
    LIBS += -L$$OUT_PWD/../lib -lopentodolist
    win32:!win32-g++: PRE_TARGETDEPS += $$OUT_PWD/../lib/opentodolist.lib
    else:unix|win32-g++: PRE_TARGETDEPS += $$OUT_PWD/../lib/libopentodolist.a

    export(TEMPLATE)
    export(CONFIG)
    export(INCLUDEPATH)
    export(LIBS)
    export(PRE_TARGETDEPS)
}

defineTest(setupStaticLib) {
    CONFIG += c++11 staticlib
    TEMPLATE = lib

    export(CONFIG)
    export(TEMPLATE)
}

defineTest(setupTest) {
    
    CONFIG += c++11
    TARGET = tst_$${1}test
    CONFIG  += console testcase
    CONFIG  -= app_bundle
    TEMPLATE = app
    DEFINES += SRCDIR=\\\"$$PWD/\\\"
    DEFINES += SRCBINDIR=\\\"$$OUT_PWD/\\\"
    QT += testlib
    QT -= gui
    INCLUDEPATH += $$PWD/../../lib
    LIBS += -L$$OUT_PWD/../../lib -lopentodolist
    win32:!win32-g++: PRE_TARGETDEPS += $$OUT_PWD/../../lib/opentodolist.lib
    else:unix|win32-g++: PRE_TARGETDEPS += $$OUT_PWD/../../lib/libopentodolist.a
    
    export(CONFIG)
    export(TARGET)
    export(TEMPLATE)
    export(DEFINES)
    export(QT)
    export(INCLUDEPATH)
    export(LIBS)
    export(PRE_TARGETDEPS)
}

#include(OpenTodoList/deploy.pri)

## Function to setup applications
#defineTest(setupApplication) {

## Enable C++11:
#CONFIG += c++11

## Location where to store assets (Depending on target platform):
#android { # Android: Automatically put into assets/ directry; leave blank
#    assets_prefix =
#} else { # All other: Save relative to application binary
#    assets_prefix = ../share/
#}

## Setup destination path
#!macx {
#DESTDIR = $$OUT_PWD/../bin
#} else {
#DESTDIR = $$OUT_PWD
#}

## Setup path (relative to install root) where to install files via make install
#target.path = /bin

#export(CONFIG)
#export(DESTDIR)
#export(target.path)
#export(assets_prefix)
#}

## Function to setup plugins
## Arg 1: Name of the plugin
## Arg 2: Type of the plugin (i.e. sub dir name below plugins/ directory)
#defineTest(setupPlugin) {
#name = $$1
#type = $$2

## Location where to store assets (Depending on target platform):
#android { # Android: Automatically put into assets/ directory; leave blank
#    assets_prefix =
#} macx { # Mac OS: Store inside app bundle
#    assets_prefix = ../../../../../../OpenTodoList/OpenTodoList.app/Contents/share/
#} else { # All other: Save relative to application binary
#    assets_prefix = ../../../share/
#}

#android:!equals(type,"opentodolist") {
#    TARGET = $$qtLibraryTarget(net_rpdev_opentodolist_plugins_$${type}_$$name)
#} else {
#    TARGET = $$qtLibraryTarget($$name)
#}
#TEMPLATE = lib
#CONFIG += plugin c++11
#macx:CONFIG -= lib_bundle
#INCLUDEPATH += ../../../OpenTodoList/inc/core
#DEPENDPATH += ../../../OpenTodoList/inc/core

## Set destination path
#macx {
#DESTDIR = $$OUT_PWD/../../../OpenTodoList/OpenTodoList.app/Contents/PlugIns/$$type
#} else {
#DESTDIR = $$OUT_PWD/../
#}

## Path to where to "make install" relative to install root
#target.path = /plugins/$$type
#target.installBase = /plugins/$$type/$$name

#export(TARGET)
#export(TEMPLATE)
#export(CONFIG)
#export(INCLUDEPATH)
#export(DEPENDPATH)
#export(DESTDIR)
#export(target.path)
#export(target.installBase)
#export(assets_prefix)

#}

RESOURCES +=



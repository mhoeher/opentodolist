# The application version
OPENTODOLIST_VERSION=3.0.0

# Pass in values via defines
DEFINES += VERSION=\\\"$$OPENTODOLIST_VERSION\\\"

QMAKE_TARGET_COMPANY = "RPdev"
QMAKE_TARGET_PRODUCT = "OpenTodoList"
QMAKE_TARGET_DESCRIPTION = "A todo and task management application"
QMAKE_TARGET_COPYRIGHT = "(c) RPdev 2014-2016"

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
    QT += testlib xml concurrent
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

# The application version
system(git describe) {
    OPENTODOLIST_VERSION = $$system(git describe)
} else {
    OPENTODOLIST_VERSION=3.0.0-unknown
}

with_update_service {
    DEFINES += WITH_UPDATE_SERVICE
}

# Pass in values via defines
DEFINES += VERSION=\\\"$$OPENTODOLIST_VERSION\\\"

QMAKE_TARGET_COMPANY = "RPdev"
QMAKE_TARGET_PRODUCT = "OpenTodoList"
QMAKE_TARGET_DESCRIPTION = "A todo and task management application"
QMAKE_TARGET_COPYRIGHT = "(c) RPdev 2014-2017"

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
QMAKE_CXXFLAGS += -Wall

CONFIG(release, debug|release) {
    DEFINES += OPENTODOLIST_RELEASE
}

CONFIG(debug, debug|release) {
    DEFINES += OPENTODOLIST_DEBUG
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

    export(CONFIG)
    export(TARGET)
    export(TEMPLATE)
    export(DEFINES)
    export(QT)
}

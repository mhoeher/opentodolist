# The application version
system(git describe --tags) {
    OPENTODOLIST_VERSION = $$system(git describe --tags)
} else {
    OPENTODOLIST_VERSION = 3.0.0-unknown
}

with_update_service {
    DEFINES *= OPENTODOLIST_WITH_UPDATE_SERVICE
}

with_appimage_extras {
    DEFINES *= OPENTODOLIST_IS_APPIMAGE
}

# Pass in values via defines
DEFINES += OPENTODOLIST_VERSION=\\\"$$OPENTODOLIST_VERSION\\\"

QMAKE_TARGET_COMPANY = "RPdev"
QMAKE_TARGET_PRODUCT = "OpenTodoList"
QMAKE_TARGET_DESCRIPTION = "A todo and task management application"
QMAKE_TARGET_COPYRIGHT = "(c) RPdev 2014-2019"

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

!equals(NEXTCLOUD_URL,"") {
    WITH_NEXTCLOUD_TEST_FLAG = "Enabled"
} else {
    WITH_NEXTCLOUD_TEST_FLAG = "Disabled"
}

!equals(OWNCLOUD_URL,"") {
    WITH_OWNCLOUD_TEST_FLAG = "Enabled"
} else {
    WITH_OWNCLOUD_TEST_FLAG = "Disabled"
}

with_update_service {
    WITH_UPDATE_SERVICE_FLAG = "Enabled"
} else {
    WITH_UPDATE_SERVICE_FLAG = "Disabled"
}

with_appimage_extras {
    WITH_APPIMAGE_EXTRAS_FLAG = "Enabled"
} else {
    WITH_APPIMAGE_EXTRAS_FLAG = "Disabled"
}

qtHaveModule(KSyntaxHighlighting) {
    WITH_KDE_SYNTAX_HIGHLIGHTING_FLAGS = "Enabled"
} else {
    WITH_KDE_SYNTAX_HIGHLIGHTING_FLAGS = "Disabled"
}


defineTest(setupTest) {

    CONFIG += c++11
    TARGET = tst_$${1}test
    CONFIG  += console testcase no_testcase_installs
    CONFIG  -= app_bundle
    TEMPLATE = app
    DEFINES += SRCDIR=\\\"$$PWD/\\\"
    DEFINES += SRCBINDIR=\\\"$$OUT_PWD/\\\"
    QT += testlib xml concurrent
    QT -= gui
    QMAKE_RPATHDIR = $$OUT_PWD/../../lib $$OUT_PWD/../../3rdparty/qlmdb/qlmdb

    export(CONFIG)
    export(TARGET)
    export(TEMPLATE)
    export(DEFINES)
    export(QT)
    export(QMAKE_RPATHDIR)
}

include(../config.pri)

QT       += qml quick xml concurrent sql remoteobjects
QT       -= gui

TEMPLATE = lib
TARGET = opentodolist-core
CONFIG += c++14 create_prl
macos:CONFIG += static

INCLUDEPATH += datamodel datastorage models

# Include QLMDB library:
INCLUDEPATH += ../3rdparty/qlmdb
win32 {
    CONFIG(release, debug|release) {
        LIBS *= -L$$OUT_PWD/../3rdparty/qlmdb/qlmdb/release
    } else {
        LIBS *= -L$$OUT_PWD/../3rdparty/qlmdb/qlmdb/debug
    }
} else {
    LIBS *= -L$$OUT_PWD/../3rdparty/qlmdb/qlmdb/
}

android {
    QT *= androidextras
}

android:versionAtLeast(QT_VERSION, 5.14.0) {
    # On Android starting with v5.14, the Qt installation is multi arch,
    # so we need to link explicitly against the correct QLMDB library:
    LIBS *= -lqlmdb_$$QT_ARCH
} else {
    LIBS *= -lqlmdb
}
QMAKE_RPATHDIR *= ../3rdparty/qlmdb/qlmdb/

SOURCES += \
    application.cpp \
    datamodel/complexitem.cpp \
    datamodel/image.cpp \
    datamodel/item.cpp \
    datamodel/library.cpp \
    datamodel/note.cpp \
    datamodel/notepage.cpp \
    datamodel/task.cpp \
    datamodel/todo.cpp \
    datamodel/todolist.cpp \
    datamodel/toplevelitem.cpp \
    datastorage/applicationsettings.cpp \
    datastorage/cache.cpp \
    datastorage/deleteitemsquery.cpp \
    datastorage/getitemquery.cpp \
    datastorage/getitemsquery.cpp \
    datastorage/getlibraryitemsuidsitemsquery.cpp \
    datastorage/getlibraryquery.cpp \
    datastorage/insertorupdateitemsquery.cpp \
    datastorage/itemsquery.cpp \
    datastorage/librariesitemsquery.cpp \
    datastorage/libraryloader.cpp \
    fileutils.cpp \
    models/itemsmodel.cpp \
    models/itemssortfiltermodel.cpp \
    models/librariesmodel.cpp \
    opentodolistqmlextensionsplugin.cpp \
    service/backgroundservice.cpp \
    sync/account.cpp \
    sync/nextcloudloginflow.cpp \
    sync/synchronizer.cpp \
    sync/syncjob.cpp \
    sync/syncrunner.cpp \
    sync/webdavclient.cpp \
    sync/webdavsynchronizer.cpp \
    utils/directorywatcher.cpp \
    utils/jsonutils.cpp \
    utils/keystore.cpp \
    utils/problem.cpp \
    utils/problemmanager.cpp \
    utils/syntaxhighlighting.cpp \
    utils/translations.cpp \
    utils/updateservice.cpp \

HEADERS += \
    abstractitemmodel.h \
    application.h \
    datamodel/complexitem.h \
    datamodel/image.h \
    datamodel/item.h \
    datamodel/library.h \
    datamodel/note.h \
    datamodel/notepage.h \
    datamodel/task.h \
    datamodel/todo.h \
    datamodel/todolist.h \
    datamodel/toplevelitem.h \
    datastorage/applicationsettings.h \
    datastorage/cache.h \
    datastorage/deleteitemsquery.h \
    datastorage/getitemquery.h \
    datastorage/getitemsquery.h \
    datastorage/getlibraryitemsuidsitemsquery.h \
    datastorage/getlibraryquery.h \
    datastorage/insertorupdateitemsquery.h \
    datastorage/itemsquery.h \
    datastorage/librariesitemsquery.h \
    datastorage/libraryloader.h \
    fileutils.h \
    models/itemsmodel.h \
    models/itemssortfiltermodel.h \
    models/librariesmodel.h \
    opentodolistqmlextensionsplugin.h \
    service/backgroundservice.h \
    sync/account.h \
    sync/nextcloudloginflow.h \
    sync/synchronizer.h \
    sync/syncjob.h \
    sync/syncrunner.h \
    sync/webdavclient.h \
    sync/webdavsynchronizer.h \
    utils/directorywatcher.h \
    utils/jsonutils.h \
    utils/keystore.h \
    utils/problem.h \
    utils/problemmanager.h \
    utils/syntaxhighlighting.h \
    utils/translations.h \
    utils/updateservice.h \

REPC_REPLICA += service/backgroundservice.rep
REPC_SOURCE += service/backgroundservice.rep

# Make git version available in code
system(git describe --tags) {
    OPENTODOLIST_VERSION = $$system(git describe --tags)
} else {
    OPENTODOLIST_VERSION = 3.0.0-unknown
}
OTHER_FILES += opentodolist_version.h.in
QMAKE_SUBSTITUTES += opentodolist_version.h.in


target.path = $$INSTALL_PREFIX$$INSTALL_SUFFIX_LIB
INSTALLS += target


android {
    QT += androidextras
    HEADERS += utils/android/androidfiledialog.h
    SOURCES += utils/android/androidfiledialog.cpp
}

!android {
    # On Android, we use a custom way to store secrets.
    config_qtkeychain {
        LIBS += -lqt5keychain
        DEFINES += OTL_USE_SYSTEM_QT5KEYCHAIN
    } else {
        include(../3rdparty/qtkeychain/qt5keychain.pri)
    }

    config_libsecret {
        # If we found libsecret, load it:
        CONFIG *= link_pkgconfig
        PKGCONFIG += libsecret-1

        # Set HAVE_LIBSECRET for qtkeychain:
        config_libsecret:DEFINES += HAVE_LIBSECRET

    }
}

include(../3rdparty/simplecrypt.pri)
include(./qmake/kf5-syntax-highlighting.pri)

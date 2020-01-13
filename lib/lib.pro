include(../config.pri)

QT       += qml quick xml concurrent sql
QT       -= gui


qtHaveModule(KSyntaxHighlighting) {
    QT += KSyntaxHighlighting
    DEFINES += HAVE_KF5_SYNTAX_HIGHLIGHTING
}


TEMPLATE = lib
TARGET = opentodolist-core
CONFIG += c++11 create_prl
macos|android:CONFIG += static

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
LIBS *= -lqlmdb
QMAKE_RPATHDIR *= ../3rdparty/qlmdb/qlmdb/

SOURCES += \
    application.cpp \
    datamodel/library.cpp \
    datamodel/item.cpp \
    datamodel/complexitem.cpp \
    datamodel/toplevelitem.cpp \
    datamodel/note.cpp \
    datamodel/image.cpp \
    datamodel/task.cpp \
    datamodel/todo.cpp \
    datamodel/todolist.cpp \
    datastorage/cache.cpp \
    datastorage/deleteitemsquery.cpp \
    datastorage/getitemquery.cpp \
    datastorage/getitemsquery.cpp \
    datastorage/getlibraryitemsuidsitemsquery.cpp \
    datastorage/getlibraryquery.cpp \
    datastorage/insertorupdateitemsquery.cpp \
    datastorage/itemsquery.cpp \
    datastorage/librariesitemsquery.cpp \
    models/librariesmodel.cpp \
    opentodolistqmlextensionsplugin.cpp \
    fileutils.cpp \
    datastorage/libraryloader.cpp \
    models/itemsmodel.cpp \
    models/itemssortfiltermodel.cpp \
    utils/directorywatcher.cpp \
    utils/keystore.cpp \
    sync/synchronizer.cpp \
    utils/jsonutils.cpp \
    sync/account.cpp \
    sync/webdavsynchronizer.cpp \
    sync/webdavclient.cpp \
    sync/syncrunner.cpp \
    sync/syncjob.cpp \
    utils/syntaxhighlighter.cpp \
    utils/translations.cpp \
    utils/updateservice.cpp

HEADERS += \
    application.h \
    datamodel/library.h \
    datamodel/item.h \
    datamodel/complexitem.h \
    datamodel/toplevelitem.h \
    datamodel/note.h \
    datamodel/image.h \
    datamodel/task.h \
    datamodel/todo.h \
    datamodel/todolist.h \
    datastorage/cache.h \
    datastorage/deleteitemsquery.h \
    datastorage/getitemquery.h \
    datastorage/getitemsquery.h \
    datastorage/getlibraryitemsuidsitemsquery.h \
    datastorage/getlibraryquery.h \
    datastorage/insertorupdateitemsquery.h \
    datastorage/itemsquery.h \
    datastorage/librariesitemsquery.h \
    models/librariesmodel.h \
    opentodolistqmlextensionsplugin.h \
    fileutils.h \
    abstractitemmodel.h \
    datastorage/libraryloader.h \
    models/itemsmodel.h \
    models/itemssortfiltermodel.h \
    utils/directorywatcher.h \
    utils/keystore.h \
    sync/synchronizer.h \
    utils/jsonutils.h \
    sync/account.h \
    sync/webdavsynchronizer.h \
    sync/webdavclient.h \
    sync/syncrunner.h \
    sync/syncjob.h \
    utils/syntaxhighlighter.h \
    utils/translations.h \
    utils/updateservice.h


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

config_qtkeychain {
    LIBS += -lqt5keychain
    DEFINES += OTL_USE_SYSTEM_QT5KEYCHAIN
} else {
    include(../3rdparty/qtkeychain/qt5keychain.pri)
}

include(../3rdparty/simplecrypt.pri)

config_libsecret {
    # If we found libsecret, load it:
    CONFIG *= link_pkgconfig
    PKGCONFIG += libsecret-1

    # Set HAVE_LIBSECRET for qtkeychain:
    config_libsecret:DEFINES += HAVE_LIBSECRET

}

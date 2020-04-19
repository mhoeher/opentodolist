INCLUDEPATH *= $$PWD $$PWD/datamodel $$PWD/datastorage $$PWD/models

QT *= qml sql

CONFIG *= link_prl

win32:CONFIG(release, debug|release) {
    LIBS *= -L$$shadowed($$PWD)/release -lcrypt32
} else:win32:CONFIG(debug, debug|release) {
    LIBS *= -L$$shadowed($$PWD)/debug
} else {
    LIBS *= -L$$shadowed($$PWD)
}

android:versionAtLeast(QT_VERSION, 5.14.0) {
    # Link against right architecture version starting with Qt 5.14 for Android
    LIBS *= -lopentodolist-core_$$QT_ARCH
} else {
    LIBS *= -lopentodolist-core
}

TEMPLATE = subdirs
CONFIG += ordered

qrc.depends =
qrc.commands = perl $$PWD/bin/mk-qrc.pl \
               --dir $$PWD/OpenTodoList/qml/ \
               --base $$PWD/OpenTodoList/ \
               --out $$PWD/OpenTodoList/OpenTodoList.qrc
QMAKE_EXTRA_TARGETS += qrc

OTHER_FILES += \
    README.md \
    COPYING \
    Doxyfile \
    doc/installers-howto.md \
    .gitlab-ci.yml \
    .qmake.conf \
    bin/* \
    cmake/* \
    templates/installer/config/config.xml \
    templates/installer/config/controller.js \
    templates/installer/packages/net.rpdev.OpenTodoList/meta/script.js \
    templates/installer/packages/net.rpdev.OpenTodoList/meta/package.xml \
    templates/appimage/default.desktop \
    $$files(templates/nsis/*,true) \
    $$files(ci/docker/android/*) \
    $$files(ci/docker/fedora/*) \
    $$files(ci/docker/fedora_system/*) \
    $$files(ci/docker/ubuntu/*) \
    $$files(ci/docker/win32/*) \
    $$files(ci/docker/win64/*) \
    $$files(ci/windows/*) \
    $$files(ci/android/*) \
    $$files(ci/*.*) \
    $$files(config.tests/qtkeychain/*) \
    $$files(config.tests/libsecret/*) \


SUBDIRS += \
    lib \
    app \
    test

include(config.pri)
# include(installer.pri)

!ios {
    # Check environment for optional packages:
    load(configure)

    qtCompileTest(qtkeychain) ## Check for QtKeychain
    config_qtkeychain {
        QT_KEYCHAIN_MODE = "System"
        LIBSECRET_MODE = "System"
    } else {
        QT_KEYCHAIN_MODE = "Built-in"
        qtCompileTest(libsecret) ## Check for libsecret
        config_libsecret:LIBSECRET_MODE = "Found"
        else:LIBSECRET_MODE = "Not Found"
    }
}


message("Configure summary:")
message("    Install Prefix:         $$INSTALL_ROOT")
message("    Binary Install Prefix:  $$INSTALL_PREFIX$$INSTALL_SUFFIX_BIN")
message("    Library Install Prefix: $$INSTALL_PREFIX$$INSTALL_SUFFIX_LIB")
message("    QtKeychain:             $$QT_KEYCHAIN_MODE")
message("    Libsecret:              $$LIBSECRET_MODE")
with_update_service {
    message("    Update Service:         ENABLED")
} else {
    message("    Update Service:         DISABLED")
}
with_nextcloud_tests {
    message("    WebDAV NextCloud Tests: ENABLED")
} else {
    message("    WebDAV NextCloud Tests: DISABLED")
}
with_appimage_extras {
    message("    AppImage Extras:        ENABLED")
} else {
    message("    AppImage Extras:        DISABLED")
}

include(appimage.pri)

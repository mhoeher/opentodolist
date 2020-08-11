TEMPLATE = subdirs

OTHER_FILES += \
    README.md \
    COPYING \
    PKGBUILD* \
    Doxyfile \
    doc/installers-howto.md \
    .gitlab-ci.yml \
    .qmake.conf \
    bin/* \
    cmake/* \
    snapcraft.yaml \
    $$files(templates/*,true) \
    $$files(ci/*,true) \
    $$files(config.tests/qtkeychain/*) \
    $$files(config.tests/libsecret/*) \
    $$files(doc/*,true) \
    $$files(net.rpdev.OpenTodoList.*)


SUBDIRS += \
    lib \
    app \
    3rdparty

lib.depends += 3rdparty
app.depends += lib

!ios:!android {
    SUBDIRS += test
    test.depends += lib
}

include(config.pri)
# include(installer.pri)

!ios:!macos {
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
message("    Update Service:         $$WITH_UPDATE_SERVICE_FLAG")
message("    Test against NextCloud: $$WITH_NEXTCLOUD_TEST_FLAG")
message("    Test against ownCloud:  $$WITH_OWNCLOUD_TEST_FLAG")
message("    AppImage Extras:        $$WITH_APPIMAGE_EXTRAS_FLAG")


include(appimage.pri)
include(qmake/qmake.pri)

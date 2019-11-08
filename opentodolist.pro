TEMPLATE = subdirs

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
    snapcraft.yaml \
    $$files(templates/nsis/*,true) \
    $$files(ci/*,true) \
    $$files(config.tests/qtkeychain/*) \
    $$files(config.tests/libsecret/*) \
    $$files(doc/*,true) \
    $$files(net.rpdev.OpenTodoList.*)


SUBDIRS += \
    lib \
    app \
    test \
    3rdparty

lib.depends += 3rdparty
app.depends += lib
test.depends += lib

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
message("    Update Service:         $$WITH_UPDATE_SERVICE_FLAG")
message("    Test against NextCloud: $$WITH_NEXTCLOUD_TEST_FLAG")
message("    Test against ownCloud:  $$WITH_OWNCLOUD_TEST_FLAG")
message("    AppImage Extras:        $$WITH_APPIMAGE_EXTRAS_FLAG")
message("    KDE Syntax Highlighter: $$WITH_KDE_SYNTAX_HIGHLIGHTING_FLAGS")


include(appimage.pri)
include(qmake/qmake.pri)

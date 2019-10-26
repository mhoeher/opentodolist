opentodolist_flatpak {
    opentodolist_flatpak_extra_files.path = $$INSTALL_PREFIX/etc/fonts/conf.d/
    opentodolist_flatpak_extra_files.files = \
        $$PWD/90-otl-color-emoji.conf
    INSTALLS += opentodolist_flatpak_extra_files
}

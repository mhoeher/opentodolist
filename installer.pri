_installer_dir = "$$OUT_PWD/installer_build"
_installer_repo = "http://www.rpdev.net/public/repositories/qtifw"
_installer_repo_linux_x86_64 = "$$_installer_repo/OpenTodoList-linux-x86_64"
_installer_repo_windows_i386 = "$$_installer_repo/OpenTodoList-windows-i386"
win32 {
    _installer_date = $$system(powershell -Command "Get-Date -format yyyy-MM-dd")
} else {
    _installer_date = $$system(date '+%Y-%m-%d')
}
_package_dir = "$$_installer_dir/packages/net.rpdev.OpenTodoList"

_installer_config = $$cat($$PWD/templates/installer/config/config.xml,blob)
_installer_config_version = \
    $$replace(_installer_config, "@@Version", $$OPENTODOLIST_VERSION)
_installer_config_linux_x86_64 = \
    $$replace(_installer_config_version, "@@RepositoryUrl", $$_installer_repo_linux_x86_64)
_installer_config_windows_i386 = \
    $$replace(_installer_config_version, "@@RepositoryUrl", $$_installer_repo_windows_i386)

_package_config = $$cat($$PWD/templates/installer/packages/net.rpdev.OpenTodoList/meta/package.xml,blob)
_package_config_version = \
    $$replace(_package_config, "@@Version", $$OPENTODOLIST_VERSION)
_package_config_date = \
    $$replace(_package_config_version, "@@ReleaseDate", "$$_installer_date")

mkpath($$OUT_PWD/installer/linux-x86_64)
mkpath($$OUT_PWD/installer/windows-i386)

write_file(\
    "$$OUT_PWD/installer/linux-x86_64/config.xml",\
    _installer_config_linux_x86_64, blob)
write_file(\
    "$$OUT_PWD/installer/windows-i386/config.xml",\
    _installer_config_windows_i386, blob)

write_file("$$OUT_PWD/installer/package.xml", _package_config_date, blob)



installer_linux_x86_64.commands = \
    @echo "Building Linux x86_64 Installer..." && \
    which patchelf && \
    rm -rf $$shell_quote($$_installer_dir) && \
    mkdir -p $$shell_quote($$_package_dir/meta) && \
    cp $$shell_quote($$OUT_PWD/installer/package.xml) \
        $$shell_quote($$_package_dir/meta) && \
    cp $$shell_quote($$PWD/COPYING) $$shell_quote($$_package_dir/meta) && \
    cp $$shell_quote($$PWD/templates/installer/packages/net.rpdev.OpenTodoList/meta/script.js) \
        $$shell_quote($$_package_dir/meta) && \
    mkdir -p $$shell_quote($$_installer_dir/config) && \
    cp $$shell_quote($$PWD/app/res/OpenTodoList80.png) \
        $$shell_quote($$_installer_dir/config/) && \
    cp $$shell_quote($$OUT_PWD/installer/linux-x86_64/config.xml) \
        $$shell_quote($$_installer_dir/config) && \
    mkdir -p $$shell_quote($$_package_dir/data) && \
    mkdir -p $$shell_quote($$_installer_dir/dist/bin) && \
    cp $$shell_quote($$OUT_PWD/app/OpenTodoList) \
        $$shell_quote($$_installer_dir/dist/bin) && \
    perl $$shell_quote($$PWD/bin/linux-deploy-qt.pl) \
        --qt $$shell_quote($$[QT_INSTALL_PREFIX]) \
        --exe $$shell_quote($$_installer_dir/dist/bin/OpenTodoList) && \
    mkdir -p $$shell_quote($$_installer_dir/dist/share/OpenTodoList/icons/) && \
    cp $$shell_quote($$PWD/app/res/OpenTodoList80.png) \
        $$shell_quote($$_installer_dir/dist/share/OpenTodoList/icons/) && \
    archivegen $$shell_quote($$_package_dir/data/OpenTodoList.7z) \
        $$shell_quote($$_installer_dir/dist/bin) \
        $$shell_quote($$_installer_dir/dist/lib) \
        $$shell_quote($$_installer_dir/dist/plugins) \
        $$shell_quote($$_installer_dir/dist/qml) \
        $$shell_quote($$_installer_dir/dist/share) \
        $$shell_quote($$_installer_dir/dist/translations) && \
    binarycreator -c $$shell_quote($$_installer_dir/config/config.xml) \
        -p $$shell_quote($$_installer_dir/packages/) \
        $$shell_quote($$_installer_dir/OpenTodoList-offline-linux-x86_64-$$OPENTODOLIST_VERSION) && \
    repogen -r \
        -p $$shell_quote($$_installer_dir/packages/) \
        $$shell_quote($$_installer_dir/OpenTodoList-linux-x86_64/) && \
    binarycreator \
        -c $$shell_quote($$_installer_dir/config/config.xml) \
        -p $$shell_quote($$_installer_dir/packages/) \
        --online-only \
        $$shell_quote($$_installer_dir/OpenTodoList-online-linux-x86_64-$$OPENTODOLIST_VERSION)


QMAKE_EXTRA_TARGETS += \
    installer_linux_x86_64



CONFIG(release, debug|release) {
    _win_build_distdir = release
    _win_deploy_switch = --release
} else {
    _win_build_distdir = debug
    _win_deploy_switch = --debug
}

installer_windows_i386.commands = \
    @echo "Building Windows i386 Installer..." && \
    IF EXIST $$shell_quote($$shell_path($$_installer_dir)) ( \
        rmdir /S /Q $$shell_quote($$shell_path($$_installer_dir)) \
    ) && \
    mkdir $$shell_quote($$shell_path($$_package_dir/meta)) && \
    copy $$shell_quote($$shell_path($$OUT_PWD/installer/package.xml)) \
        $$shell_quote($$shell_path($$_package_dir/meta)) && \
    copy $$shell_quote($$shell_path($$PWD/COPYING)) \
        $$shell_quote($$shell_path($$_package_dir/meta)) && \
    copy $$shell_quote($$shell_path($$PWD/templates/installer/packages/net.rpdev.OpenTodoList/meta/script.js)) \
        $$shell_quote($$shell_path($$_package_dir/meta)) && \
    mkdir $$shell_quote($$shell_path($$_installer_dir/config)) && \
    copy $$shell_quote($$shell_path($$PWD/app/res/OpenTodoList80.png)) \
        $$shell_quote($$shell_path($$_installer_dir/config/)) && \
    copy $$shell_quote($$shell_path($$OUT_PWD/installer/windows-i386/config.xml)) \
        $$shell_quote($$shell_path($$_installer_dir/config)) && \
    mkdir $$shell_quote($$shell_path($$_package_dir/data)) && \
    mkdir $$shell_quote($$shell_path($$_installer_dir/dist/bin)) && \
    copy $$shell_quote($$shell_path($$OUT_PWD/app/$$_win_build_distdir/OpenTodoList.exe)) \
        $$shell_quote($$shell_path($$_installer_dir/dist/bin)) && \
    windeployqt $$_win_deploy_switch --qmldir $$shell_quote($$shell_path($$PWD/app)) \
        $$shell_quote($$shell_path($$_installer_dir/dist/bin/OpenTodoList.exe)) && \
    mkdir $$shell_quote($$shell_path($$_installer_dir/dist/share/OpenTodoList/icons/)) && \
    copy $$shell_quote($$shell_path($$PWD/app/res/OpenTodoList.ico)) \
        $$shell_quote($$shell_path($$_installer_dir/dist/share/OpenTodoList/icons/)) && \
    archivegen $$shell_quote($$shell_path($$_package_dir/data/OpenTodoList.7z)) \
        $$shell_quote($$shell_path($$_installer_dir/dist/bin)) \
        $$shell_quote($$shell_path($$_installer_dir/dist/lib)) \
        $$shell_quote($$shell_path($$_installer_dir/dist/plugins)) \
        $$shell_quote($$shell_path($$_installer_dir/dist/qml)) \
        $$shell_quote($$shell_path($$_installer_dir/dist/share)) \
        $$shell_quote($$shell_path($$_installer_dir/dist/translations)) && \
    binarycreator -c $$shell_quote($$shell_path($$_installer_dir/config/config.xml)) \
        -p $$shell_quote($$shell_path($$_installer_dir/packages/)) \
        $$shell_quote($$shell_path($$_installer_dir/OpenTodoList-offline-windows-i386-$${OPENTODOLIST_VERSION}.exe)) && \
    repogen -r \
        -p $$shell_quote($$shell_path($$_installer_dir/packages/)) \
        $$shell_quote($$shell_path($$_installer_dir/OpenTodoList-windows-i386/)) && \
    binarycreator \
        -c $$shell_quote($$shell_path($$_installer_dir/config/config.xml)) \
        -p $$shell_quote($$shell_path($$_installer_dir/packages/)) \
        --online-only \
        $$shell_quote($$shell_path($$_installer_dir/OpenTodoList-online-windows-i386-$$OPENTODOLIST_VERSION)) && \
    @echo Installer build finished


QMAKE_EXTRA_TARGETS += \
    installer_windows_i386

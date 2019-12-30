# On UNIX like systems, install a *.desktop file:

unix {
    desktop.files += $$PWD/net.rpdev.OpenTodoList.desktop
    desktop.path = $$INSTALL_PREFIX/share/applications
    INSTALLS += desktop
}

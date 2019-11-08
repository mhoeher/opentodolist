# Install AppData XML file:

system(gzip --version) {
    in_file = $$PWD/../net.rpdev.OpenTodoList.appdata.xml
    out_file = $$INSTALL_PREFIX/share/app-info/xmls/net.rpdev.OpenTodoList.appdata.xml.gz
    appdata.extra = gzip --best > $$out_file < $$in_file
} else {
    appdata.files = $$PWD/../net.rpdev.OpenTodoList.appdata.xml
}

appdata.path = $$INSTALL_PREFIX/share/app-info/xmls

INSTALLS += appdata

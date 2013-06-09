TEMPLATE = subdirs
SUBDIRS = OpenTodoListCore OpenTodoListDesktopQml plugins

isEmpty(INSTALL_PREFIX){
    INSTALL_PREFIX = $$OUT_PWD
}

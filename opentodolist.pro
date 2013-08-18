TEMPLATE = subdirs
SUBDIRS = OpenTodoListCore plugins OpenTodoList

isEmpty(INSTALL_PREFIX) {
    INSTALL_PREFIX = $$OUT_PWD
}

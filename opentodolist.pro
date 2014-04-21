TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS = OpenTodoListCore plugins OpenTodoList

OpenTodoList.depends += OpenTodoListCore

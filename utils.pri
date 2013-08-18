# Copies the given files to the destination directory
# Taken from: http://stackoverflow.com/questions/3984104/qmake-how-to-copy-a-file-to-the-output
# with some own modifications with respect to keeping the directory structure intakt
defineTest(copyToDestDir) {
    files = $$1
    sourceroot = $$2

    for(FILE, files) {
        DDIR = $$DESTDIR/$$FILE
        SFILE = $$sourceroot/$$FILE

        # Replace slashes in paths with backslashes for Windows
        win32:SFILE ~= s,/,\\,g
        win32:DDIR ~= s,/,\\,g

        win32 {
            QMAKE_POST_LINK += $$QMAKE_CHK_DIR_EXISTS $$dirname(DDIR) $$QMAKE_MKDIR $$dirname(DDIR) $$escape_expand(\\n\\t)
        } else {
            QMAKE_POST_LINK += $$QMAKE_MKDIR $$dirname(DDIR) $$escape_expand(\\n\\t)
        }
        QMAKE_POST_LINK += $$QMAKE_COPY $$quote($$SFILE) $$quote($$DDIR) $$escape_expand(\\n\\t)
    }

    export(QMAKE_POST_LINK)
}

defineTest(pluginConfig) {
    type = $$1
    macx|android {
        CONFIG += static
    } else {
        DESTDIR = ../../../OpenTodoList/plugins/$$type
    }

    INCLUDEPATH += $$PWD/../../../OpenTodoListCore
    LIBS += -L../../../OpenTodoListCore -lOpenTodoListCore

    target.path = $$INSTALL_PREFIX/lib/OpenTodoList/plugins/$$type
    INSTALLS += target

    export(CONFIG)
    export(DESTDIR)
    export(INCLUDEPATH)
    export(LIBS)
    export(target)
    export(INSTALLS)
}

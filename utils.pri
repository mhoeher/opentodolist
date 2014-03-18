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

    INCLUDEPATH += $$PWD/../../../OpenTodoListCore
    macx {
        DESTDIR = ../../../OpenTodoList/OpenTodoList.app/Contents/MacOs/plugins/$$type
        QMAKE_POST_LINK = install_name_tool -change libOpenTodoListCore.0.dylib \
                      @executable_path/../Frameworks/libOpenTodoListCore.0.dylib \
                      $$DESTDIR/${TARGET}
    } else {
        DESTDIR = ../../../OpenTodoList/plugins/$$type
    }

    android {
        x86 {
            target.path = /libs/x86/plugins/$$type
        } else:armeabi {
            target.path = /libs/armeabi/plugins/$$type
        } else {
            target.path = /libs/armeabi-v7a/plugins/$$type
        }
        INSTALLS += target

        # FIXME: Ideally, make plugins also *.so files but for now compile them statically
        CONFIG += static
    }

    export(CONFIG)
    export(DESTDIR)
    export(INCLUDEPATH)
    export(target.path)
    export(INSTALLS)
    export(QMAKE_POST_LINK)
}

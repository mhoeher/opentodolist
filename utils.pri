# Copies the given files to the destination directory
# Taken from: http://stackoverflow.com/questions/3984104/qmake-how-to-copy-a-file-to-the-output
# with some own modifications with respect to keeping the directory structure intakt
defineTest(copyToDestDir) {
    files = $$1

    for(FILE, files) {
        DDIR = $$DESTDIR/$$FILE

        # Replace slashes in paths with backslashes for Windows
        win32:FILE ~= s,/,\\,g
        win32:DDIR ~= s,/,\\,g

        QMAKE_POST_LINK += $$QMAKE_CHK_DIR_EXISTS $$dirname(DDIR) $$QMAKE_MKDIR $$dirname(DDIR) $$escape_expand(\\n\\t)
        QMAKE_POST_LINK += $$QMAKE_COPY $$quote($$FILE) $$quote($$DDIR) $$escape_expand(\\n\\t)
    }

    export(QMAKE_POST_LINK)
}

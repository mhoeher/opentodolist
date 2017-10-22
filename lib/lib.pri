INCLUDEPATH *= $$PWD $$PWD/datamodel $$PWD/datastorage $$PWD/models

QT *= qml sql

CONFIG *= link_prl

win32:!win32-g++: OTL_LIBRARY_NAME += opentodolist.lib
else:unix|win32-g++: OTL_LIBRARY_NAME += libopentodolist.a


win32:CONFIG(release, debug|release) {
    LIBS *= -L$$shadowed($$PWD)/release -lcrypt32
    PRE_TARGETDEPS *= $$shadowed($$PWD)/release/$$OTL_LIBRARY_NAME
} else:win32:CONFIG(debug, debug|release) {
    LIBS *= -L$$shadowed($$PWD)/debug
    PRE_TARGETDEPS *= $$shadowed($$PWD)/debug/$$OTL_LIBRARY_NAME
} else {
    LIBS *= -L$$shadowed($$PWD)
    PRE_TARGETDEPS *= $$shadowed($$PWD)/$$OTL_LIBRARY_NAME
}

LIBS *= -lopentodolist

INCLUDEPATH += $$PWD $$PWD/datamodel $$PWD/datastorage $$PWD/models
LIBS += -L$$shadowed($$PWD) -lopentodolist

QT += qml

win32:!win32-g++: PRE_TARGETDEPS += $$shadowed($$PWD)/opentodolist.lib
else:unix|win32-g++: PRE_TARGETDEPS += $$shadowed($$PWD)/libopentodolist.a

# Link against crypt32 on Windows when using
# built-in qtkeychain:
!config_qtkeychain {
    win32:LIBS *= -lcrypt32
}

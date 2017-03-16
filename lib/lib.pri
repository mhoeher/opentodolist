INCLUDEPATH += $$PWD $$PWD/datamodel
LIBS += -L$$OUT_PWD/../lib -lopentodolist

win32:!win32-g++: PRE_TARGETDEPS += $$OUT_PWD/../lib/opentodolist.lib
else:unix|win32-g++: PRE_TARGETDEPS += $$OUT_PWD/../lib/libopentodolist.a

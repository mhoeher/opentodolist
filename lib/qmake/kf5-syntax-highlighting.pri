# Pull in the KDE Syntax Highlighting library into
# the library as in-source.
HEADERS *= \
    $$files($$PWD/../../3rdparty/KDE/syntax-highlighting/src/lib/*.h) \
    $$files($$PWD/kf5-syntax-highlighting/*.h)
SOURCES *= \
    $$files($$PWD/../../3rdparty/KDE/syntax-highlighting/src/lib/*.cpp) \
    $$files($$PWD/kf5-syntax-highlighting/*.cpp)
RESOURCES *= \
    $$PWD/../../3rdparty/KDE/syntax-highlighting/data/themes/theme-data.qrc \
    $$PWD/kf5-syntax-highlighting/syntax-data.qrc
DEFINES *= KF5SyntaxHighlighting_EXPORTS
INCLUDEPATH *= \
    $$PWD/../../3rdparty/KDE/syntax-highlighting/src/lib \
    $$PWD/kf5-syntax-highlighting

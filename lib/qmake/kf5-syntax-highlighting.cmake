# Pull in the KDE Syntax Highlighting library into
# the library as in-source.

file(
    GLOB 
    headers1 
    ${CMAKE_CURRENT_LIST_DIR}/../../3rdparty/KDE/syntax-highlighting/src/lib/*.h
)
file(
    GLOB
    headers2
    ${CMAKE_CURRENT_LIST_DIR}/kf5-syntax-highlighting/*.h
)

file(
    GLOB
    sources1
    ${CMAKE_CURRENT_LIST_DIR}/../../3rdparty/KDE/syntax-highlighting/src/lib/*.cpp
)
file(
    GLOB
    sources2
    ${CMAKE_CURRENT_LIST_DIR}/kf5-syntax-highlighting/*.cpp
)
    
set(KF5_SYNTAXHIGHLIGHTING_HEADERS ${headers1} ${headers2})
set(KF5_SYNTAXHIGHLIGHTING_SOURCES ${sources1} ${sources2})
set(KF5_SYNTAXHIGHLIGHTING_RESOURCES
    ${CMAKE_CURRENT_LIST_DIR}/../../3rdparty/KDE/syntax-highlighting/data/themes/theme-data.qrc
    ${CMAKE_CURRENT_LIST_DIR}/kf5-syntax-highlighting/syntax-data.qrc
)
set(KF5_SYNTAXHIGHLIGHTING_DEFINES KF5SyntaxHighlighting_EXPORTS)
set(KF5_SYNTAXHIGHLIGHTING_INCLUDEPATHS
    ${CMAKE_CURRENT_LIST_DIR}/../../3rdparty/KDE/syntax-highlighting/src/lib
    ${CMAKE_CURRENT_LIST_DIR}/kf5-syntax-highlighting
)


unset(headers1)
unset(headers2)
unset(sources1)
unset(sources2)

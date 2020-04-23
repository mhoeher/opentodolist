# Instruct CMake to run moc and rrc automatically when needed.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

if(ANDROID)
    set(LIB_HEADERS_ANDROID
        ./utils/android/androidfiledialog.h)
    set(LIB_SOURCES_ANDROID
        ./utils/android/androidfiledialog.cpp)
endif()


set(LIB_HEADERS
    abstractitemmodel.h
    application.h
    datamodel/complexitem.h
    datamodel/image.h
    datamodel/item.h
    datamodel/library.h
    datamodel/note.h
    datamodel/notepage.h
    datamodel/task.h
    datamodel/todo.h
    datamodel/todolist.h
    datamodel/toplevelitem.h
    datastorage/cache.h
    datastorage/deleteitemsquery.h
    datastorage/getitemsquery.h
    datastorage/getitemquery.h
    datastorage/getlibraryquery.h
    datastorage/getlibraryitemsuidsitemsquery.h
    datastorage/insertorupdateitemsquery.h
    datastorage/itemsquery.h
    datastorage/librariesitemsquery.h
    datastorage/libraryloader.h
    fileutils.h
    models/itemsmodel.h
    models/itemssortfiltermodel.h
    models/librariesmodel.h
    opentodolistqmlextensionsplugin.h
    sync/account.h
    sync/synchronizer.h
    sync/syncjob.h
    sync/syncrunner.h
    sync/webdavclient.h
    sync/webdavsynchronizer.h
    utils/directorywatcher.h
    utils/jsonutils.h
    utils/keystore.h
    utils/updateservice.h
    utils/syntaxhighlighter.h
    utils/problem.h
    utils/problemmanager.h
    utils/translations.h

    ${LIB_HEADERS_ANDROID}
)

set(LIB_SOURCES
    application.cpp
    datamodel/complexitem.cpp
    datamodel/image.cpp
    datamodel/item.cpp
    datamodel/library.cpp
    datamodel/note.cpp
    datamodel/notepage.cpp
    datamodel/task.cpp
    datamodel/todo.cpp
    datamodel/todolist.cpp
    datamodel/toplevelitem.cpp
    datastorage/cache.cpp
    datastorage/deleteitemsquery.cpp
    datastorage/getitemsquery.cpp
    datastorage/getitemquery.cpp
    datastorage/getlibraryquery.cpp
    datastorage/getlibraryitemsuidsitemsquery.cpp
    datastorage/insertorupdateitemsquery.cpp
    datastorage/itemsquery.cpp
    datastorage/librariesitemsquery.cpp
    datastorage/libraryloader.cpp
    fileutils.cpp
    models/itemsmodel.cpp
    models/itemssortfiltermodel.cpp
    models/librariesmodel.cpp
    opentodolistqmlextensionsplugin.cpp
    sync/account.cpp
    sync/synchronizer.cpp
    sync/syncjob.cpp
    sync/syncrunner.cpp
    sync/webdavclient.cpp
    sync/webdavsynchronizer.cpp
    utils/directorywatcher.cpp
    utils/jsonutils.cpp
    utils/keystore.cpp
    utils/updateservice.cpp
    utils/syntaxhighlighter.cpp
    utils/problem.cpp
    utils/problemmanager.cpp
    utils/translations.cpp

    ${LIB_SOURCES_ANDROID}

    # Include SimpleCrypt source:
    ${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/simplecrypt/simplecrypt.cpp
)


# Find required Qt libraries
find_package(Qt5Qml REQUIRED)
find_package(Qt5Quick REQUIRED)
find_package(Qt5Xml REQUIRED)
find_package(Qt5Concurrent REQUIRED)
find_package(Qt5Sql REQUIRED)

# Find KDE Syntax Highlighting engine
find_package(KF5SyntaxHighlighting)
if(KF5SyntaxHighlighting_FOUND)
    set(LIB_LIBRARIES ${LIB_LIBRARIES} KF5::SyntaxHighlighting)
    set(LIB_DEFINES ${LIB_DEFINES} HAVE_KF5_SYNTAX_HIGHLIGHTING)
endif()


if(ANDROID OR WIN32)
    set(libtype STATIC)
else()
    set(libtype SHARED)
endif()
# Add the library
add_library(opentodolist-common ${libtype}
    ${LIB_HEADERS}
    ${LIB_SOURCES}
)

# Set include paths
target_include_directories(
    opentodolist-common
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty
        ${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/simplecrypt
        ${CMAKE_CURRENT_BINARY_DIR}/../3rdparty/qtkeychain
)

# Add compile definitions
target_compile_definitions(opentodolist-common PRIVATE ${LIB_DEFINES})

# Link against Qt libraries
target_link_libraries(opentodolist-common

    PUBLIC

    # Qt libraries:
    Qt5::Qml
    Qt5::Quick
    Qt5::Xml
    Qt5::Concurrent
    Qt5::Sql

    # Third party libraries:
    qt5keychain
    qlmdb

    # Optional libraries
    ${LIB_LIBRARIES}
)


install(
    TARGETS opentodolist-common
    DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

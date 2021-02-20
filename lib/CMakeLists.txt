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
    datastorage/applicationsettings.h
    datastorage/cache.h
    datastorage/deleteitemsquery.h
    datastorage/getitemquery.h
    datastorage/getitemsquery.h
    datastorage/getlibraryitemsuidsitemsquery.h
    datastorage/getlibraryquery.h
    datastorage/insertorupdateitemsquery.h
    datastorage/itemsquery.h
    datastorage/librariesitemsquery.h
    datastorage/libraryloader.h
    datastorage/movetodoquery.h
    fileutils.h
    models/itemsmodel.h
    models/itemssortfiltermodel.h
    models/librariesmodel.h
    opentodolistqmlextensionsplugin.h
    service/backgroundservice.h
    sync/account.h
    sync/synchronizer.h
    sync/syncjob.h
    sync/syncrunner.h
    sync/webdavsynchronizer.h
    utils/directorywatcher.h
    utils/jsonutils.h
    utils/keystore.h
    utils/problem.h
    utils/problemmanager.h
    utils/syntaxhighlighting.h
    utils/translations.h
    utils/updateservice.h

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
    datastorage/applicationsettings.cpp
    datastorage/cache.cpp
    datastorage/deleteitemsquery.cpp
    datastorage/getitemquery.cpp
    datastorage/getitemsquery.cpp
    datastorage/getlibraryitemsuidsitemsquery.cpp
    datastorage/getlibraryquery.cpp
    datastorage/insertorupdateitemsquery.cpp
    datastorage/itemsquery.cpp
    datastorage/librariesitemsquery.cpp
    datastorage/libraryloader.cpp
    datastorage/movetodoquery.cpp
    fileutils.cpp
    models/itemsmodel.cpp
    models/itemssortfiltermodel.cpp
    models/librariesmodel.cpp
    opentodolistqmlextensionsplugin.cpp
    service/backgroundservice.cpp
    sync/account.cpp
    sync/synchronizer.cpp
    sync/syncjob.cpp
    sync/syncrunner.cpp
    sync/webdavsynchronizer.cpp
    utils/directorywatcher.cpp
    utils/jsonutils.cpp
    utils/keystore.cpp
    utils/problem.cpp
    utils/problemmanager.cpp
    utils/syntaxhighlighting.cpp
    utils/translations.cpp
    utils/updateservice.cpp

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
find_package(Qt5RemoteObjects REQUIRED)

# Find KDE Syntax Highlighting engine
# find_package(KF5SyntaxHighlighting)
# if(KF5SyntaxHighlighting_FOUND)
#     set(LIB_LIBRARIES ${LIB_LIBRARIES} KF5::SyntaxHighlighting)
#     set(LIB_DEFINES ${LIB_DEFINES} HAVE_KF5_SYNTAX_HIGHLIGHTING)
# endif()
include(qmake/kf5-syntax-highlighting.cmake)
set(LIB_HEADERS ${LIB_HEADERS} ${KF5_SYNTAXHIGHLIGHTING_HEADERS})
set(
    LIB_SOURCES ${LIB_SOURCES} 
    ${KF5_SYNTAXHIGHLIGHTING_SOURCES} ${KF5_SYNTAXHIGHLIGHTING_RESOURCES}
)

qt5_generate_repc(LIB_SOURCES service/backgroundservice.rep SOURCE)
qt5_generate_repc(LIB_SOURCES service/backgroundservice.rep REPLICA)

# Add the library
add_library(opentodolist-common
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
        ${CMAKE_CURRENT_BINARY_DIR}
    PRIVATE
        ${KF5_SYNTAXHIGHLIGHTING_INCLUDEPATHS}
)

# Add compile definitions
target_compile_definitions(
    opentodolist-common 
    PRIVATE 
        ${LIB_DEFINES}
        ${KF5_SYNTAXHIGHLIGHTING_DEFINES}
)

# Link against Qt libraries
target_link_libraries(opentodolist-common

    PUBLIC

    # Qt libraries:
    Qt5::Qml
    Qt5::Quick
    Qt5::Xml
    Qt5::Concurrent
    Qt5::Sql
    Qt5::RemoteObjects

    # Third party libraries:
    qt5keychain
    qlmdb
    synqclient

    # Optional libraries
    ${LIB_LIBRARIES}
)


install(
    TARGETS opentodolist-common
    DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

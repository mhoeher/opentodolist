# Instruct CMake to run moc and rrc automatically when needed.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)


set(LIB_HEADERS
    ./opentodolistqmlextensionsplugin.h
    ./models/itemsmodel.h
    ./models/itemssortfiltermodel.h
    ./fileutils.h
    ./application.h
    ./sync/webdavclient.h
    ./sync/syncjob.h
    ./sync/synchronizer.h
    ./sync/syncrunner.h
    ./sync/webdavsynchronizer.h
    ./utils/jsonutils.h
    ./utils/keystore.h
    ./utils/directorywatcher.h
    ./utils/updateservice.h
    ./abstractitemmodel.h
    ./migrators/migrator_2_x_to_3_x.h
    ./datamodel/complexitem.h
    ./datamodel/todo.h
    ./datamodel/item.h
    ./datamodel/todolist.h
    ./datamodel/image.h
    ./datamodel/toplevelitem.h
    ./datamodel/library.h
    ./datamodel/task.h
    ./datamodel/note.h
    ./datastorage/libraryloader.h
    ./datastorage/itemcontainer.h
)

set(LIB_SOURCES
    ./models/itemsmodel.cpp
    ./models/itemssortfiltermodel.cpp
    ./fileutils.cpp
    ./sync/webdavsynchronizer.cpp
    ./sync/webdavclient.cpp
    ./sync/synchronizer.cpp
    ./sync/syncrunner.cpp
    ./sync/syncjob.cpp
    ./utils/jsonutils.cpp
    ./utils/keystore.cpp
    ./utils/directorywatcher.cpp
    ./utils/updateservice.cpp
    ./opentodolistqmlextensionsplugin.cpp
    ./migrators/migrator_2_x_to_3_x.cpp
    ./datamodel/library.cpp
    ./datamodel/item.cpp
    ./datamodel/todo.cpp
    ./datamodel/complexitem.cpp
    ./datamodel/toplevelitem.cpp
    ./datamodel/todolist.cpp
    ./datamodel/task.cpp
    ./datamodel/image.cpp
    ./datamodel/note.cpp
    ./application.cpp
    ./datastorage/itemcontainer.cpp
    ./datastorage/libraryloader.cpp

    # Include SimpleCrypt source:
    ${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/simplecrypt/simplecrypt.cpp
)

# Android specific
#     ./utils/android/androidfiledialog.h
#     ./utils/android/androidfiledialog.cpp


# Find required Qt libraries
find_package(Qt5Qml REQUIRED)
find_package(Qt5Quick REQUIRED)
find_package(Qt5Xml REQUIRED)
find_package(Qt5Concurrent REQUIRED)
find_package(Qt5Sql REQUIRED)

# Add the library
add_library(opentodolist-common STATIC
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
)

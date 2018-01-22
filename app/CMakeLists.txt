# Instruct CMake to run moc and rrc automatically when needed.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(APP_SOURCES
    main.cpp
)

set(APP_RESOURCES
    ./qml.qrc
    ./res.qrc
    ./translations.qrc
)

# Find required Qt libraries
find_package(Qt5Qml REQUIRED)
find_package(Qt5Quick REQUIRED)
find_package(Qt5Xml REQUIRED)
find_package(Qt5Concurrent REQUIRED)
find_package(Qt5Widgets REQUIRED)

if(WIN32)
    set(APP_FLAGS WIN32)
endif()

if(APPLE)
    set(APP_FLAGS MACOSX_BUNDLE)
    set(APP_SOURCES ${APP_SOURCES} res/OpenTodoList.icns)
    set(MACOSX_BUNDLE_ICON_FILE OpenTodoList.icns)
    set_source_files_properties(res/OpenTodoList.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
endif()

add_executable(OpenTodoList ${APP_FLAGS} ${APP_SOURCES} ${APP_RESOURCES})

target_link_libraries(
    OpenTodoList

    # Internal:
    opentodolist-common

    # Qt libraries:
    Qt5::Qml
    Qt5::Quick
    Qt5::Xml
    Qt5::Concurrent
    Qt5::Widgets
)

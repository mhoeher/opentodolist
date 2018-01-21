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

add_executable(OpenTodoList WIN32 ${APP_SOURCES} ${APP_RESOURCES})

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

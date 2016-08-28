file(GLOB lib_sources 
    *.cpp
    datamodel/*.cpp
)
file(GLOB lib_headers
    *.h
    datamodel/*.h
)

include_directories(datamodel)

# Create config file:
configure_file(opentodolist_config.h.in 
    ${CMAKE_CURRENT_BINARY_DIR}/opentodolist_config.h)

find_package(Qt5Core REQUIRED)
find_package(Qt5Qml REQUIRED)
find_package(Qt5Concurrent REQUIRED)
find_package(Qt5Quick REQUIRED)
find_package(Qt5Xml REQUIRED)

add_library(OpenTodoListCore STATIC ${lib_sources} ${lib_headers})
target_link_libraries(
    OpenTodoListCore
    Qt5::Core
    Qt5::Qml
    Qt5::Concurrent
    Qt5::Quick
    Qt5::Xml
)

# Enable C++11:
set_property(TARGET OpenTodoListCore PROPERTY CXX_STANDARD 11)
set_property(TARGET OpenTodoListCore PROPERTY CXX_STANDARD_REQUIRED ON)

file(GLOB app_sources *.cpp)
file(GLOB app_headers *.h)
file(GLOB app_resources *.qrc)

find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)

if(WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mwindows")
endif()

add_executable(OpenTodoList ${app_sources} ${app_headers} ${app_resources})
target_include_directories(OpenTodoList 
    PRIVATE 
        ../lib
        ${CMAKE_CURRENT_BINARY_DIR}/../lib
)
target_link_libraries(
    OpenTodoList
    OpenTodoListCore
    Qt5::Core
    Qt5::Gui
)

# Enable C++11:
set_property(TARGET OpenTodoList PROPERTY CXX_STANDARD 11)
set_property(TARGET OpenTodoList PROPERTY CXX_STANDARD_REQUIRED ON)

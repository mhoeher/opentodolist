file(GLOB app_sources *.cpp)
file(GLOB app_headers *.h)
file(GLOB app_resources *.qrc)

# Include other files (QML, resources, ...):
file(GLOB_RECURSE app_3rdpartyinfo 3rdpartyinfo/*)
file(GLOB_RECURSE app_android android/*)
file(GLOB_RECURSE app_qml net/*)
file(GLOB_RECURSE app_res res/*)
set(app_otherfiles
    ${app_3rdpartyinfo}
    ${app_android}
    ${app_qml}
    ${app_res}
    ../COPYING
    ../README.md
    ../Doxyfile
    ../doc/installers-howto.md
    ../.gitlab-ci.yml
    ../ci/docker/fedora/Dockerfile
)
foreach(file ${app_otherfiles})
    set_source_files_properties(${file} PROPERTIES HEADER_FILE_ONLY TRUE)
endforeach()

find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)

if(WIN32)
    # Build a GUI app:
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mwindows")
    
    # To make the resource compiler find the icon:
    include_directories(${CMAKE_CURRENT_SOURCE_DIR})
    
    # Include the Windows resource file (to set icon of exe):
    set(app_sources 
        ${app_sources} 
        OpenTodoList.rc)
endif()

add_executable(OpenTodoList ${app_sources} ${app_headers} ${app_resources} ${app_otherfiles})
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

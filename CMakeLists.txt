cmake_minimum_required(VERSION 3.5.1)

# Generate build commands JSON file:
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Auto-detect Android build from within Qt Creator:
include(./3rdparty/qt-cmake-android/cmake/autodetect-android-build.cmake)

project(opentodolist)

# Include modules:
include(cmake/options.cmake)
include(cmake/version.cmake)
include(cmake/BuildLinuxAppImage.cmake)
include(cmake/Flatpak.cmake)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Enable unit tests:
enable_testing()



if(OPENTODOLIST_WITH_UPDATE_SERVICE)
    add_definitions(-DOPENTODOLIST_WITH_UPDATE_SERVICE)
endif()
if(OPENTODOLIST_WITH_APPIMAGE_EXTRAS)
    add_definitions(-DOPENTODOLIST_IS_APPIMAGE)
endif()


# Set C++ standard to use:
set(CMAKE_CXX_STANDARD 14)

# Enable warnings:
if(MSVC)
  # Force to always compile with W4
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/Wall" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Wall")
  endif()
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  # Update if necessary
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
endif()


# Set options of included 3rd party libraries:
set(QLMDB_WITH_STATIC_LIBS ON CACHE BOOL "Build QLMDB as static library")
set(QLMDB_WITH_BUILTIN_LMDB ON CACHE BOOL "Use built in lmdb library")
set(QLMDB_WITHOUT_TESTS ON CACHE BOOL "Do not build QLMDB unit tests")
set(SYNQCLIENT_WITHOUT_TESTS ON CACHE BOOL "Disable tests in SynqClient")
set(QTKEYCHAIN_STATIC ON CACHE BOOL "Build static QtKeychain library")

# Build using static libraries:
set(BUILD_SHARED_LIBS OFF)

# Install data files:
set(APPLICATION_NAME OpenTodoList)
set(APPLICATION_EXECUTABLE OpenTodoList)
set(APPLICATION_ICON_NAME net.rpdev.OpenTodoList)
configure_file(
    net.rpdev.OpenTodoList.desktop.in
    ${CMAKE_CURRENT_BINARY_DIR}/net.rpdev.OpenTodoList.desktop
)
include(GNUInstallDirs)
install(
    DIRECTORY app/icons/hicolor
    DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/
)
install(
    FILES net.rpdev.OpenTodoList.appdata.xml
    DESTINATION ${CMAKE_INSTALL_DATADIR}/appdata
)
install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/net.rpdev.OpenTodoList.desktop
    DESTINATION ${CMAKE_INSTALL_DATADIR}/applications
)

# Disable building translations in QtKeychain:
set(BUILD_TRANSLATIONS NO CACHE BOOL "")


# Include sub-directories:
add_subdirectory(3rdparty)
add_subdirectory(lib)
add_subdirectory(app)
add_subdirectory(test)


# Show other, project-related files:
file(GLOB_RECURSE OTHER_FILES
    OpenTodoList.desktop.in
    README.md
    COPYING
    Doxyfile
    net.rpdev.OpenTodoList.json
    net.rpdev.OpenTodoList.appdata.xml
    snapcraft.yaml
    doc/*
    .gitlab-ci.yml
    bin/*
    templates/*
    ci/*
    cmake/*
    app/android/*
)

add_custom_target(OtherFiles SOURCES ${OTHER_FILES})

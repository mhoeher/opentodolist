cmake_minimum_required(VERSION 3.5.1)

# Generate build commands JSON file:
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(OpenTodoList)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


# Include modules:
include(cmake/options.cmake)
include(cmake/version.cmake)
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

# Add additional files for Android:
set(
    ANDROID_PACKAGE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/app/android
    CACHE INTERNAL "Extra files for Android deployment"
)

if(ANDROID_TARGET_SDK_VERSION)
    set(
        ANDROID_TARGET_SDK_VERSION ${ANDROID_TARGET_SDK_VERSION}
        CACHE INTERNAL "Android Target SDK version to build against."
    )
endif()

if (ANDROID)
    include(3rdparty/android-openssl/CMakeLists.txt)
endif()


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

if(NOT OPENTODOLIST_QT_VERSIONS)
    # If OPENTODOLIST_QT_VERSIONS is not set by user, first, look for Qt6, followed by Qt5:
    set(OPENTODOLIST_QT_VERSIONS Qt6 Qt5)
endif()

set(REQUIRED_QT_COMPONENTS Core Quick Qml Xml Concurrent Sql RemoteObjects LinguistTools Widgets Network NetworkAuth)

if(ANDROID)
    set(REQUIRED_QT_COMPONENTS ${REQUIRED_QT_COMPONENTS} AndroidExtras WebView)
endif()

if(${CMAKE_SYSTEM_NAME} STREQUAL "iOS")
    set(REQUIRED_QT_COMPONENTS ${REQUIRED_QT_COMPONENTS} WebView)
endif()

find_package(QT NAMES ${OPENTODOLIST_QT_VERSIONS} COMPONENTS ${REQUIRED_QT_COMPONENTS} REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS ${REQUIRED_QT_COMPONENTS} REQUIRED)

if(NOT ANDROID)
    if(OPENTODOLIST_USE_SYSTEM_QTKEYCHAIN)
        find_package(Qt${QT_VERSION_MAJOR}Keychain REQUIRED)
    endif()
endif()

if(OPENTODOLIST_USE_SYSTEM_QLMDB)
    find_package(QLMDB-qt${QT_VERSION_MAJOR} REQUIRED)
endif()

if(OPENTODOLIST_USE_SYSTEM_SYNQCLIENT)
    find_package(SynqClient-qt${QT_VERSION_MAJOR} REQUIRED)
endif()

if(OPENTODOLIST_USE_SYSTEM_KF_SYNTAX_HIGHLIGHTING)
    find_package(KF5SyntaxHighlighting)
endif()

if(OPENTODOLIST_WITH_KNOTIFICATIONS)
    find_package(KF5Notifications REQUIRED)
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
if(NOT ${CMAKE_SYSTEM_NAME} STREQUAL "iOS")
    add_subdirectory(test)
endif()

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

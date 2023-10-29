cmake_minimum_required(VERSION 3.16)

# Generate build commands JSON file:
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Enable running Qt specific tools automatically (otherwise, we have to
# wrap stuff in Qt specific functions). See
# https://www.kdab.com/wp-content/uploads/stories/QTVTC20-Using-Modern-CMake-Kevin-Funk.pdf
# for more details.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_VISIBILITY_PRESET hidden) # To avoid linker warnings building for iOS

# Set policies explicitly:
# cmake_policy(SET CMP0114 NEW) # Use independent step targets w/ external project on Xcode new build system.

project(OpenTodoList)


# Include modules:
include(cmake/options.cmake)
include(cmake/version.cmake)
include(cmake/Flatpak.cmake)
include(3rdparty/android-openssl/android_openssl.cmake)

# Enable unit tests:
enable_testing()

if(NOT OPENTODOLIST_QT_VERSIONS)
    # If OPENTODOLIST_QT_VERSIONS is not set by user, first, look for Qt6, followed by Qt5:
    set(OPENTODOLIST_QT_VERSIONS Qt6 Qt5)
endif()

set(REQUIRED_QT_COMPONENTS Core Quick Qml Xml Concurrent Sql RemoteObjects LinguistTools Widgets Network NetworkAuth)


find_package(QT NAMES ${OPENTODOLIST_QT_VERSIONS} COMPONENTS ${REQUIRED_QT_COMPONENTS} REQUIRED)

if(ANDROID)
    if(QT_VERSION_MAJOR STREQUAL "5")
        message(FATAL_ERROR "Support for building OpenTodoList for Android using Qt5 has been dropped. Please build using Qt6 or higher.")
    endif()
endif()

find_package(Qt${QT_VERSION_MAJOR} COMPONENTS ${REQUIRED_QT_COMPONENTS} REQUIRED)

# Convenience: Set the QT_INSTALL_PREFIX based on QT_DIR (which is set by the first find_package
# call above. The QT_DIR is set to something like
# <prefix>/lib/cmake/Qt6, so we have to get the directory 3 times.
if(NOT QT_INSTALL_PREFIX)
    get_filename_component(QT_INSTALL_PREFIX ${QT_DIR} DIRECTORY)
    get_filename_component(QT_INSTALL_PREFIX ${QT_INSTALL_PREFIX} DIRECTORY)
    get_filename_component(QT_INSTALL_PREFIX ${QT_INSTALL_PREFIX} DIRECTORY)
    if(QT_INSTALL_PREFIX STREQUAL "/")
      set(QT_INSTALL_PREFIX "")
    endif()
endif()

# Convenience: Set a path to a host Qt installation (for cross compiling). This is needed when
# we cross-compile as we have to build some tools for the host.
if(NOT QT_HOST_INSTALL_PREFIX)
    set(QT_HOST_INSTALL_PREFIX ${QT_INSTALL_PREFIX})
    if(ANDROID OR IOS)
        # Check if this is a Qt installation made with the official installer
        get_filename_component(_version_dir ${QT_INSTALL_PREFIX} DIRECTORY)
        get_filename_component(_root_dir ${_version_dir} DIRECTORY)
        if(EXISTS ${_root_dir}/MaintenanceTool.dat)
            if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin")
                set(QT_HOST_INSTALL_PREFIX ${_version_dir}/macos)
            elseif(CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
                set(QT_HOST_INSTALL_PREFIX ${_version_dir}/gcc_64)
            elseif(CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
                message("TODO: Implement auto-detection of Qt host tools on Windows")
            endif()
        endif()
        unset(_version_dir)
        unset(_root_dir)
    endif()
endif()

# For KDE packages: Used when cross compiling
if(NOT KF5_HOST_TOOLING)
    set(KF5_HOST_TOOLING ${QT_HOST_INSTALL_PREFIX})
endif()


if(OPENTODOLIST_WITH_KNOTIFICATIONS)
    find_package(KF5Notifications REQUIRED)
endif()

if(OPENTODOLIST_USE_SYSTEM_KF_SYNTAX_HIGHLIGHTING)
    find_package(KF5SyntaxHighlighting REQUIRED)
endif()

if(NOT ANDROID AND OPENTODOLIST_USE_SYSTEM_QTKEYCHAIN)
    find_package(Qt${QT_VERSION_MAJOR}Keychain REQUIRED)
endif()

if(OPENTODOLIST_USE_SYSTEM_QLMDB)
    find_package(QLMDB-qt${QT_VERSION_MAJOR} REQUIRED)
endif()

if(OPENTODOLIST_USE_SYSTEM_SYNQCLIENT)
    find_package(SynqClient-qt${QT_VERSION_MAJOR} REQUIRED)
endif()

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
    FILES net.rpdev.OpenTodoList.metainfo.xml
    DESTINATION ${CMAKE_INSTALL_DATADIR}/metainfo
)
install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/net.rpdev.OpenTodoList.desktop
    DESTINATION ${CMAKE_INSTALL_DATADIR}/applications
)


# Include sub-directories:
add_subdirectory(3rdparty)
add_subdirectory(lib)
add_subdirectory(app)
if(NOT IOS)
    add_subdirectory(test)
endif()


# Show other, project-related files:
file(GLOB_RECURSE OTHER_FILES
    OpenTodoList.desktop.in
    README.md
    COPYING
    Doxyfile
    net.rpdev.OpenTodoList.json
    net.rpdev.OpenTodoList.metainfo.xml
    snapcraft.yaml
    doc/*
    .gitlab-ci.yml
    bin/*
    templates/*
    ci/*
    cmake/*
    app/android/*
    lib/*
    test/*
)

add_custom_target(OtherFiles SOURCES ${OTHER_FILES})

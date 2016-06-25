cmake_minimum_required(VERSION 3.4.1)
project(OpenTodoList)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(OPENTODOLIST_MAJOR_VERSION 2)
set(OPENTODOLIST_MINOR_VERSION 8)
set(OPENTODOLIST_PATCH_VERSION 0)
set(OPENTODOLIST_VERSION 
    ${OPENTODOLIST_MAJOR_VERSION}.${OPENTODOLIST_MINOR_VERSION}.${OPENTODOLIST_PATCH_VERSION})


if(CMAKE_SYSTEM_NAME STREQUAL Android)
    find_package(ECM)
    set(ANDROID 1)
endif()


add_subdirectory(lib)
add_subdirectory(app)

# Enable testing:
enable_testing()
add_subdirectory(test)

# Get QT_INSTALL_PREFIX:
find_package(Qt5Core REQUIRED)
get_target_property(QT_INSTALL_PREFIX Qt5::Core LOCATION)
get_filename_component(QT_INSTALL_PREFIX ${QT_INSTALL_PREFIX} DIRECTORY)
get_filename_component(QT_INSTALL_PREFIX ${QT_INSTALL_PREFIX} DIRECTORY)

# Build installers:
set(INSTALLER_DIR ${CMAKE_CURRENT_BINARY_DIR}/installer_build)
set(INSTALLER_REPO http://www.rpdev.net/public/repositories/qtifw)
string(TIMESTAMP INSTALLER_DATE "%Y-%m-%d")
set(INSTALLER_APPLICATIONS_DIR @ApplicationsDir@)
set(INSTALLER_TARGET_DIR @TargetDir@)
set(INSTALLER_CONTROLLER_SCRIPT
    ${CMAKE_CURRENT_SOURCE_DIR}/templates/installer/config/controller.js
)
set(INSTALLER_ONLINE_REPOSITORY_ROOT http://rpdev.net/public/repositories/qtifw/)
if (UNIX AND NOT ANDROID)
    include(BuildLinuxInstaller.cmake)
endif()
if (WIN32)
    include(BuildWindowsInstaller.cmake)
endif()

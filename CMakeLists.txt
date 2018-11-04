cmake_minimum_required(VERSION 3.5.1)

project(opentodolist)

# Include modules:
include(cmake/options.cmake)
include(cmake/version.cmake)
include(cmake/BuildLinuxAppImage.cmake)
include(cmake/qt-android-mk-apk.cmake)

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
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic")
endif()


# Configure the build in qlmdb library:
set(QLMDB_WITH_STATIC_LIBS ON CACHE BOOL "Build QLMDB as static library")
set(QLMDB_WITH_BUILTIN_LMDB ON CACHE BOOL "Use built in lmdb library")


# Include sub-directories:
add_subdirectory(3rdparty)
add_subdirectory(lib)
add_subdirectory(app)
add_subdirectory(test)


# Show other, project-related files:
file(GLOB_RECURSE OTHER_FILES
    README.md
    COPYING
    Doxyfile
    doc/*
    .gitlab-ci.yml
    bin/*
    templates/*
    ci/*
    cmake/*
    app/android/*
)

add_custom_target(OtherFiles SOURCES ${OTHER_FILES})

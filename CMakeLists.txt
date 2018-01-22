cmake_minimum_required(VERSION 2.8.11)

project(opentodolist)

# Include modules:
include(cmake/options.cmake)
include(cmake/version.cmake)
include(cmake/BuildLinuxAppImage.cmake)

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
    doc/installers-howto.md
    .gitlab-ci.yml
    bin/*
    templates/*
    ci/*
)

add_custom_target(OtherFiles SOURCES ${OTHER_FILES})

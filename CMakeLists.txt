cmake_minimum_required(VERSION 2.8.8)
project(OpenTodoList)

# Let CMake run moc when necessary:
set(CMAKE_AUTOMOC ON)

# Always include current directory in search path as tools as moc
# put files there:
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5Core REQUIRED)

add_subdirectory(OpenTodoListCore)
add_subdirectory(OpenTodoListDesktopQml)
add_subdirectory(plugins)

file(GLOB test_sources *.cpp)
find_package(Qt5Test REQUIRED)

add_executable(test_task ${test_sources})
target_link_libraries(test_task OpenTodoListCore Qt5::Test)
target_include_directories(test_task PRIVATE ../../lib)
add_test(NAME task COMMAND $<TARGET_FILE:test_task>)

# Enable C++11:
set_property(TARGET test_task PROPERTY CXX_STANDARD 11)
set_property(TARGET test_task PROPERTY CXX_STANDARD_REQUIRED ON)

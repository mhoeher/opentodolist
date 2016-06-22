file(GLOB test_sources *.cpp)
find_package(Qt5Test REQUIRED)

add_executable(test_todo ${test_sources})
target_link_libraries(test_todo OpenTodoListCore Qt5::Test)
target_include_directories(test_todo PRIVATE ../../lib)
add_test(NAME todo COMMAND $<TARGET_FILE:test_todo>)

# Enable C++11:
set_property(TARGET test_todo PROPERTY CXX_STANDARD 11)
set_property(TARGET test_todo PROPERTY CXX_STANDARD_REQUIRED ON)

file(GLOB test_sources *.cpp)
find_package(Qt5Test REQUIRED)

add_executable(test_todolist ${test_sources})
target_link_libraries(test_todolist OpenTodoListCore Qt5::Test)
target_include_directories(test_todolist PRIVATE ../../lib)
add_test(NAME todolist COMMAND $<TARGET_FILE:test_todolist>)

# Enable C++11:
set_property(TARGET test_todolist PROPERTY CXX_STANDARD 11)
set_property(TARGET test_todolist PROPERTY CXX_STANDARD_REQUIRED ON)

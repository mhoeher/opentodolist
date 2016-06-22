file(GLOB test_sources *.cpp)
find_package(Qt5Test REQUIRED)

add_executable(test_note ${test_sources})
target_link_libraries(test_note OpenTodoListCore Qt5::Test)
target_include_directories(test_note PRIVATE ../../lib)
add_test(NAME note COMMAND $<TARGET_FILE:test_note>)

# Enable C++11:
set_property(TARGET test_note PROPERTY CXX_STANDARD 11)
set_property(TARGET test_note PROPERTY CXX_STANDARD_REQUIRED ON)

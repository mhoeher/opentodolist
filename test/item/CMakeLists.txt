file(GLOB test_sources *.cpp)
find_package(Qt5Test REQUIRED)

add_executable(test_item ${test_sources})
target_link_libraries(test_item OpenTodoListCore Qt5::Test)
target_include_directories(test_item PRIVATE ../../lib)
add_test(NAME item COMMAND $<TARGET_FILE:test_item>)

# Enable C++11:
set_property(TARGET test_item PROPERTY CXX_STANDARD 11)
set_property(TARGET test_item PROPERTY CXX_STANDARD_REQUIRED ON)

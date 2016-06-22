file(GLOB test_sources *.cpp)
find_package(Qt5Test REQUIRED)

add_executable(test_toplevelitem ${test_sources})
target_link_libraries(test_toplevelitem OpenTodoListCore Qt5::Test)
target_include_directories(test_toplevelitem PRIVATE ../../lib)
add_test(NAME toplevelitem COMMAND $<TARGET_FILE:test_toplevelitem>)

# Enable C++11:
set_property(TARGET test_toplevelitem PROPERTY CXX_STANDARD 11)
set_property(TARGET test_toplevelitem PROPERTY CXX_STANDARD_REQUIRED ON)

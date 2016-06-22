file(GLOB test_sources *.cpp)
find_package(Qt5Test REQUIRED)

add_executable(test_complexitem ${test_sources})
target_link_libraries(test_complexitem OpenTodoListCore Qt5::Test)
target_include_directories(test_complexitem PRIVATE ../../lib)
add_test(NAME complexitem COMMAND $<TARGET_FILE:test_complexitem>)

# Enable C++11:
set_property(TARGET test_complexitem PROPERTY CXX_STANDARD 11)
set_property(TARGET test_complexitem PROPERTY CXX_STANDARD_REQUIRED ON)

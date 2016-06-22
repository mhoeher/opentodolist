file(GLOB test_sources *.cpp)
find_package(Qt5Test REQUIRED)

add_executable(test_library ${test_sources})
target_link_libraries(test_library OpenTodoListCore Qt5::Test)
target_include_directories(test_library PRIVATE ../../lib)
add_test(NAME library COMMAND $<TARGET_FILE:test_library>)

# Enable C++11:
set_property(TARGET test_library PROPERTY CXX_STANDARD 11)
set_property(TARGET test_library PROPERTY CXX_STANDARD_REQUIRED ON)

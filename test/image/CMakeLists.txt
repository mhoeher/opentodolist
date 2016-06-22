file(GLOB test_sources *.cpp)
find_package(Qt5Test REQUIRED)

add_executable(test_image ${test_sources})
target_link_libraries(test_image OpenTodoListCore Qt5::Test)
target_include_directories(test_image PRIVATE ../../lib)
add_test(NAME image COMMAND $<TARGET_FILE:test_image>)

# Enable C++11:
set_property(TARGET test_image PROPERTY CXX_STANDARD 11)
set_property(TARGET test_image PROPERTY CXX_STANDARD_REQUIRED ON)

file(GLOB test_sources *.cpp)
find_package(Qt5Test REQUIRED)

add_executable(test_application ${test_sources})
target_link_libraries(test_application OpenTodoListCore Qt5::Test)
target_include_directories(test_application PRIVATE ../../lib)
add_test(NAME application COMMAND $<TARGET_FILE:test_application>)

# Enable C++11:
set_property(TARGET test_application PROPERTY CXX_STANDARD 11)
set_property(TARGET test_application PROPERTY CXX_STANDARD_REQUIRED ON)

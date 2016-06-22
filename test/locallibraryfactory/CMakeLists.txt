file(GLOB test_sources *.cpp)
find_package(Qt5Test REQUIRED)

add_executable(test_locallibraryfactory ${test_sources})
target_link_libraries(test_locallibraryfactory OpenTodoListCore Qt5::Test)
target_include_directories(test_locallibraryfactory PRIVATE ../../lib)
add_test(NAME locallibraryfactory COMMAND $<TARGET_FILE:test_locallibraryfactory>)

# Enable C++11:
set_property(TARGET test_locallibraryfactory PROPERTY CXX_STANDARD 11)
set_property(TARGET test_locallibraryfactory PROPERTY CXX_STANDARD_REQUIRED ON)

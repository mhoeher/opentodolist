find_package(Qt5Test REQUIRED)
add_executable(
    test_library

    test_library.cpp
)

target_link_libraries(
    test_library

    # Internal libraries:
    opentodolist-common

    # Qt Libraries
    Qt5::Test
)

add_test(
    NAME library
    COMMAND $<TARGET_FILE:test_library> -platform minimal
)

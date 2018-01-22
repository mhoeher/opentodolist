find_package(Qt5Test REQUIRED)
add_executable(
    test_jsonutils

    test_jsonutils.cpp
)

target_link_libraries(
    test_jsonutils

    # Internal libraries:
    opentodolist-common

    # Qt Libraries
    Qt5::Test
)

add_test(
    NAME jsonutils
    COMMAND $<TARGET_FILE:test_jsonutils> -platform minimal
)

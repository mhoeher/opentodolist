find_package(Qt${QT_VERSION_MAJOR}Test REQUIRED)
add_executable(
    test_jsonutils

    test_jsonutils.cpp
)

target_link_libraries(
    test_jsonutils

    # Internal libraries:
    opentodolist-common

    # Qt Libraries
    Qt::Test
)

add_test(
    NAME jsonutils
    COMMAND $<TARGET_FILE:test_jsonutils> -platform minimal
)

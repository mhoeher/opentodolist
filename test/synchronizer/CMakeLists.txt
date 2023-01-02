find_package(Qt${QT_VERSION_MAJOR}Test REQUIRED)
add_executable(
    test_synchronizer

    test_synchronizer.cpp
)

target_link_libraries(
    test_synchronizer

    # Internal libraries:
    opentodolist-common

    # Qt Libraries
    Qt::Test
)

add_test(
    NAME synchronizer
    COMMAND $<TARGET_FILE:test_synchronizer> -platform minimal
)

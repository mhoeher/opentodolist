find_package(Qt${QT_VERSION_MAJOR}Test REQUIRED)
add_executable(
    test_keystore

    test_keystore.cpp
)

target_link_libraries(
    test_keystore

    # Internal libraries:
    opentodolist-common

    # Qt Libraries
    Qt::Test
)

add_test(
    NAME keystore
    COMMAND $<TARGET_FILE:test_keystore> -platform minimal
)

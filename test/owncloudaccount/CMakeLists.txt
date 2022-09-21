find_package(Qt${QT_VERSION_MAJOR}Test REQUIRED)
add_executable(
    test_owncloudaccount

    test_owncloudaccount.cpp
)

target_link_libraries(
    test_owncloudaccount

    # Internal libraries:
    opentodolist-common

    # Qt Libraries
    Qt::Test
)

add_test(
    NAME owncloudaccount
    COMMAND $<TARGET_FILE:test_owncloudaccount> -platform minimal
)

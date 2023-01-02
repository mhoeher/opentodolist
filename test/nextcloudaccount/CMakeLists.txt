find_package(Qt${QT_VERSION_MAJOR}Test REQUIRED)
add_executable(
    test_nextcloudaccount

    test_nextcloudaccount.cpp
)

target_link_libraries(
    test_nextcloudaccount

    # Internal libraries:
    opentodolist-common

    # Qt Libraries
    Qt::Test
)

add_test(
    NAME nextcloudaccount
    COMMAND $<TARGET_FILE:test_nextcloudaccount> -platform minimal
)

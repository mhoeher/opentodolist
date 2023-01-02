find_package(Qt${QT_VERSION_MAJOR}Test REQUIRED)
add_executable(
    test_movetodoquery

    test_movetodoquery.cpp
)

target_link_libraries(
    test_movetodoquery

    # Internal libraries:
    opentodolist-common

    # Qt Libraries
    Qt::Test
)

add_test(
    NAME movetodoquery
    COMMAND $<TARGET_FILE:test_movetodoquery> -platform minimal
)

find_package(Qt${QT_VERSION_MAJOR}Test REQUIRED)
add_executable(
    test_updateitemquery

    test_updateitemquery.cpp
)

target_link_libraries(
    test_updateitemquery

    # Internal libraries:
    opentodolist-common

    # Qt Libraries
    Qt::Test
)

add_test(
    NAME updateitemquery
    COMMAND $<TARGET_FILE:test_updateitemquery> -platform minimal
)

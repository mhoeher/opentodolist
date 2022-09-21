find_package(Qt${QT_VERSION_MAJOR}Test REQUIRED)
add_executable(
    test_getitemsquery

    test_getitemsquery.cpp
)

target_link_libraries(
    test_getitemsquery

    # Internal libraries:
    opentodolist-common

    # Qt Libraries
    Qt::Test
)

add_test(
    NAME getitemsquery
    COMMAND $<TARGET_FILE:test_getitemsquery> -platform minimal
)

find_package(Qt${QT_VERSION_MAJOR}Test REQUIRED)
add_executable(
    test_deleteitemsquery

    test_deleteitemsquery.cpp
)

target_link_libraries(
    test_deleteitemsquery

    # Internal libraries:
    opentodolist-common

    # Qt Libraries
    Qt::Test
)

add_test(
    NAME deleteitemsquery
    COMMAND $<TARGET_FILE:test_deleteitemsquery> -platform minimal
)

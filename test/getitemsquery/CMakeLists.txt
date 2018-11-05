find_package(Qt5Test REQUIRED)
add_executable(
    test_getitemsquery

    test_getitemsquery.cpp
)

target_link_libraries(
    test_getitemsquery

    # Internal libraries:
    opentodolist-common

    # Qt Libraries
    Qt5::Test
)

add_test(
    NAME getitemsquery
    COMMAND $<TARGET_FILE:test_getitemsquery> -platform minimal
)

find_package(Qt5Test REQUIRED)
add_executable(
    test_insertorupdateitemsquery

    test_insertorupdateitemsquery.cpp
)

target_link_libraries(
    test_insertorupdateitemsquery

    # Internal libraries:
    opentodolist-common

    # Qt Libraries
    Qt5::Test
)

add_test(
    NAME insertorupdateitemsquery
    COMMAND $<TARGET_FILE:test_insertorupdateitemsquery> -platform minimal
)

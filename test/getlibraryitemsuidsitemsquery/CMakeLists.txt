find_package(Qt5Test REQUIRED)
add_executable(
    test_getlibraryitemsuidsitemsquery

    test_getlibraryitemsuidsitemsquery.cpp
)

target_link_libraries(
    test_getlibraryitemsuidsitemsquery

    # Internal libraries:
    opentodolist-common

    # Qt Libraries
    Qt5::Test
)

add_test(
    NAME getlibraryitemsuidsitemsquery
    COMMAND $<TARGET_FILE:test_getlibraryitemsuidsitemsquery> -platform minimal
)

find_package(Qt${QT_VERSION_MAJOR}Test REQUIRED)
add_executable(
    test_getlibraryitemsuidsitemsquery

    test_getlibraryitemsuidsitemsquery.cpp
)

target_link_libraries(
    test_getlibraryitemsuidsitemsquery

    # Internal libraries:
    opentodolist-common

    # Qt Libraries
    Qt::Test
)

add_test(
    NAME getlibraryitemsuidsitemsquery
    COMMAND $<TARGET_FILE:test_getlibraryitemsuidsitemsquery> -platform minimal
)

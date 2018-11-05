find_package(Qt5Test REQUIRED)
add_executable(
    test_getlibraryquery

    test_getlibraryquery.cpp
)

target_link_libraries(
    test_getlibraryquery

    # Internal libraries:
    opentodolist-common

    # Qt Libraries
    Qt5::Test
)

add_test(
    NAME getlibraryquery
    COMMAND $<TARGET_FILE:test_getlibraryquery> -platform minimal
)

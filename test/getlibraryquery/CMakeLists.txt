find_package(Qt${QT_VERSION_MAJOR}Test REQUIRED)
add_executable(
    test_getlibraryquery

    test_getlibraryquery.cpp
)

target_link_libraries(
    test_getlibraryquery

    # Internal libraries:
    opentodolist-common

    # Qt Libraries
    Qt::Test
)

add_test(
    NAME getlibraryquery
    COMMAND $<TARGET_FILE:test_getlibraryquery> -platform minimal
)

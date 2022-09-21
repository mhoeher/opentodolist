find_package(Qt${QT_VERSION_MAJOR}Test REQUIRED)
add_executable(
    test_getitemquery

    test_getitemquery.cpp
)

target_link_libraries(
    test_getitemquery

    # Internal libraries:
    opentodolist-common

    # Qt Libraries
    Qt::Test
)

add_test(
    NAME getitemquery
    COMMAND $<TARGET_FILE:test_getitemquery> -platform minimal
)

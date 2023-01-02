find_package(Qt${QT_VERSION_MAJOR}Test REQUIRED)
add_executable(
    test_promotetaskquery

    test_promotetaskquery.cpp
)

target_link_libraries(
    test_promotetaskquery

    # Internal libraries:
    opentodolist-common

    # Qt Libraries
    Qt::Test
)

add_test(
    NAME promotetaskquery
    COMMAND $<TARGET_FILE:test_promotetaskquery> -platform minimal
)

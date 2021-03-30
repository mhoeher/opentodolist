find_package(Qt5Test REQUIRED)
add_executable(
    test_promotetaskquery

    test_promotetaskquery.cpp
)

target_link_libraries(
    test_promotetaskquery

    # Internal libraries:
    opentodolist-common

    # Qt Libraries
    Qt5::Test
)

add_test(
    NAME promotetaskquery
    COMMAND $<TARGET_FILE:test_promotetaskquery> -platform minimal
)

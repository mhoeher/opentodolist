find_package(Qt5Test REQUIRED)
add_executable(
    test_item

    test_item.cpp
)

target_link_libraries(
    test_item

    # Internal libraries:
    opentodolist-common

    # Qt Libraries
    Qt5::Test
)

add_test(
    NAME item
    COMMAND $<TARGET_FILE:test_item> -platform minimal
)

find_package(Qt${QT_VERSION_MAJOR}Test REQUIRED)
add_executable(
    test_item

    test_item.cpp
)

target_link_libraries(
    test_item

    # Internal libraries:
    opentodolist-common

    # Qt Libraries
    Qt::Test
)

add_test(
    NAME item
    COMMAND $<TARGET_FILE:test_item> -platform minimal
)

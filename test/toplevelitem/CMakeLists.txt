find_package(Qt5Test REQUIRED)
add_executable(
    test_toplevelitem

    test_toplevelitem.cpp
)

target_link_libraries(
    test_toplevelitem

    # Internal libraries:
    opentodolist-common

    # Qt Libraries
    Qt5::Test
)

add_test(
    NAME toplevelitem
    COMMAND $<TARGET_FILE:test_toplevelitem> -platform minimal
)

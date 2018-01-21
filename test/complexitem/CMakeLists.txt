find_package(Qt5Test REQUIRED)
add_executable(
    test_complexitem

    test_complexitem.cpp
)

target_link_libraries(
    test_complexitem

    # Internal libraries:
    opentodolist-common

    # Qt Libraries
    Qt5::Test
)

add_test(
    NAME complexitem
    COMMAND $<TARGET_FILE:test_complexitem> -platform minimal
)

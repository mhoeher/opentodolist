find_package(Qt${QT_VERSION_MAJOR}Test REQUIRED)
add_executable(
    test_complexitem

    test_complexitem.cpp
)

target_link_libraries(
    test_complexitem

    # Internal libraries:
    opentodolist-common

    # Qt Libraries
    Qt::Test
)

add_test(
    NAME complexitem
    COMMAND $<TARGET_FILE:test_complexitem> -platform minimal
)

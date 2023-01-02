find_package(Qt${QT_VERSION_MAJOR}Test REQUIRED)
add_executable(
    test_toplevelitem

    test_toplevelitem.cpp
)

target_link_libraries(
    test_toplevelitem

    # Internal libraries:
    opentodolist-common

    # Qt Libraries
    Qt::Test
)

add_test(
    NAME toplevelitem
    COMMAND $<TARGET_FILE:test_toplevelitem> -platform minimal
)

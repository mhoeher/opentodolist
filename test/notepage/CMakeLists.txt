find_package(Qt${QT_VERSION_MAJOR}Test REQUIRED)
add_executable(
    test_notepage

    test_notepage.cpp
)

target_link_libraries(
    test_notepage

    # Internal libraries:
    opentodolist-common

    # Qt Libraries
    Qt::Test
)

add_test(
    NAME notepage
    COMMAND $<TARGET_FILE:test_notepage> -platform minimal
)

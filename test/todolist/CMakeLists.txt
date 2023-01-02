find_package(Qt${QT_VERSION_MAJOR}Test REQUIRED)
add_executable(
    test_todolist

    test_todolist.cpp
)

target_link_libraries(
    test_todolist

    # Internal libraries:
    opentodolist-common

    # Qt Libraries
    Qt::Test
)

add_test(
    NAME todolist
    COMMAND $<TARGET_FILE:test_todolist> -platform minimal
)

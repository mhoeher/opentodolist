find_package(Qt5Test REQUIRED)
add_executable(
    test_todolist

    test_todolist.cpp
)

target_link_libraries(
    test_todolist

    # Internal libraries:
    opentodolist-common

    # Qt Libraries
    Qt5::Test
)

add_test(
    NAME todolist
    COMMAND $<TARGET_FILE:test_todolist> -platform minimal
)

find_package(Qt5Test REQUIRED)
add_executable(
    test_todo

    test_todo.cpp
)

target_link_libraries(
    test_todo

    # Internal libraries:
    opentodolist-common

    # Qt Libraries
    Qt5::Test
)

add_test(
    NAME todo
    COMMAND $<TARGET_FILE:test_todo> -platform minimal
)

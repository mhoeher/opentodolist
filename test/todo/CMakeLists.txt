find_package(Qt${QT_VERSION_MAJOR}Test REQUIRED)
add_executable(
    test_todo

    test_todo.cpp
)

target_link_libraries(
    test_todo

    # Internal libraries:
    opentodolist-common

    # Qt Libraries
    Qt::Test
)

add_test(
    NAME todo
    COMMAND $<TARGET_FILE:test_todo> -platform minimal
)

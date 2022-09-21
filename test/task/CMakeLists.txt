find_package(Qt${QT_VERSION_MAJOR}Test REQUIRED)
add_executable(
    test_task

    test_task.cpp
)

target_link_libraries(
    test_task

    # Internal libraries:
    opentodolist-common

    # Qt Libraries
    Qt::Test
)

add_test(
    NAME task
    COMMAND $<TARGET_FILE:test_task> -platform minimal
)

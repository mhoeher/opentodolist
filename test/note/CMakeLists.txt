find_package(Qt${QT_VERSION_MAJOR}Test REQUIRED)
add_executable(
    test_note

    test_note.cpp
)

target_link_libraries(
    test_note

    # Internal libraries:
    opentodolist-common

    # Qt Libraries
    Qt::Test
)

add_test(
    NAME note
    COMMAND $<TARGET_FILE:test_note> -platform minimal
)

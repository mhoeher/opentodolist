find_package(Qt5Test REQUIRED)
add_executable(
    test_note

    test_note.cpp
)

target_link_libraries(
    test_note

    # Internal libraries:
    opentodolist-common

    # Qt Libraries
    Qt5::Test
)

add_test(
    NAME complexitem
    COMMAND $<TARGET_FILE:test_note> -platform minimal
)

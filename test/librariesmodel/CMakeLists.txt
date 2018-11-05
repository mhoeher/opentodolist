find_package(Qt5Test REQUIRED)
add_executable(
    test_librariesmodel

    test_librariesmodel.cpp
)

target_link_libraries(
    test_librariesmodel

    # Internal libraries:
    opentodolist-common

    # Qt Libraries
    Qt5::Test
)

add_test(
    NAME librariesmodel
    COMMAND $<TARGET_FILE:test_librariesmodel> -platform minimal
)

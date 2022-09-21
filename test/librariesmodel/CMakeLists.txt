find_package(Qt${QT_VERSION_MAJOR}Test REQUIRED)
add_executable(
    test_librariesmodel

    test_librariesmodel.cpp
)

target_link_libraries(
    test_librariesmodel

    # Internal libraries:
    opentodolist-common

    # Qt Libraries
    Qt::Test
)

add_test(
    NAME librariesmodel
    COMMAND $<TARGET_FILE:test_librariesmodel> -platform minimal
)

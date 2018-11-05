find_package(Qt5Test REQUIRED)
add_executable(
    test_librariesitemsquery

    test_librariesitemsquery.cpp
)

target_link_libraries(
    test_librariesitemsquery

    # Internal libraries:
    opentodolist-common

    # Qt Libraries
    Qt5::Test
)

add_test(
    NAME librariesitemsquery
    COMMAND $<TARGET_FILE:test_librariesitemsquery> -platform minimal
)

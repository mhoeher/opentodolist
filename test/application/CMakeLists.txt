find_package(Qt5Test REQUIRED)
add_executable(
    test_application

    test_application.cpp
)

target_link_libraries(
    test_application

    # Internal libraries:
    opentodolist-common

    # Qt Libraries
    Qt5::Test
)

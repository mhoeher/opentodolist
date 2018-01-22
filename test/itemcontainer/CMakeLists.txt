find_package(Qt5Test REQUIRED)
add_executable(
    test_itemcontainer

    test_itemcontainer.cpp
)

target_link_libraries(
    test_itemcontainer

    # Internal libraries:
    opentodolist-common

    # Qt Libraries
    Qt5::Test
)

add_test(
    NAME itemcontainer
    COMMAND $<TARGET_FILE:test_itemcontainer> -platform minimal
)

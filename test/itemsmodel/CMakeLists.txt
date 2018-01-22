find_package(Qt5Test REQUIRED)
add_executable(
    test_itemsmodel

    test_itemsmodel.cpp
)

target_link_libraries(
    test_itemsmodel

    # Internal libraries:
    opentodolist-common

    # Qt Libraries
    Qt5::Test
)

add_test(
    NAME itemsmodel
    COMMAND $<TARGET_FILE:test_itemsmodel> -platform minimal
)

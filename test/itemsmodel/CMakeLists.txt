find_package(Qt${QT_VERSION_MAJOR}Test REQUIRED)
add_executable(
    test_itemsmodel

    test_itemsmodel.cpp
)

target_link_libraries(
    test_itemsmodel

    # Internal libraries:
    opentodolist-common

    # Qt Libraries
    Qt::Test
)

add_test(
    NAME itemsmodel
    COMMAND $<TARGET_FILE:test_itemsmodel> -platform minimal
)

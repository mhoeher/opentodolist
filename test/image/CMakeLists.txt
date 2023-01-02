find_package(Qt${QT_VERSION_MAJOR}Test REQUIRED)
add_executable(
    test_image

    test_image.cpp
)

target_link_libraries(
    test_image

    # Internal libraries:
    opentodolist-common

    # Qt Libraries
    Qt::Test
)

target_compile_definitions(
    test_image
    PRIVATE
        -DSRCDIR="${CMAKE_CURRENT_SOURCE_DIR}")

add_test(
    NAME image
    COMMAND $<TARGET_FILE:test_image> -platform minimal
)

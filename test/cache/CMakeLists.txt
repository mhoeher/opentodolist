find_package(Qt${QT_VERSION_MAJOR}Test REQUIRED)
add_executable(
    test_cache

    test_cache.cpp
)

target_link_libraries(
    test_cache

    # Internal libraries:
    opentodolist-common

    # Qt Libraries
    Qt::Test
)

add_test(
    NAME cache
    COMMAND $<TARGET_FILE:test_cache> -platform minimal
)

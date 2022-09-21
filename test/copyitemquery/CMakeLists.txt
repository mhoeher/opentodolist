find_package(Qt${QT_VERSION_MAJOR}Test REQUIRED)
add_executable(
    test_copyitemquery

    test_copyitemquery.cpp
)

target_link_libraries(
    test_copyitemquery

    # Internal libraries:
    opentodolist-common

    # Qt Libraries
    Qt::Test
)

target_compile_definitions(
    test_copyitemquery
    PRIVATE
        SAMPLES_PATH="${CMAKE_CURRENT_SOURCE_DIR}"
)

add_test(
    NAME copyitemquery
    COMMAND $<TARGET_FILE:test_copyitemquery> -platform minimal -nocrashhandler
)

find_package(Qt${QT_VERSION_MAJOR}Test REQUIRED)
add_executable(
    test_webdavsynchronizer

    test_webdavsynchronizer.cpp
)

target_link_libraries(
    test_webdavsynchronizer

    # Internal libraries:
    opentodolist-common

    # Qt Libraries
    Qt::Test
)

add_test(
    NAME webdavsynchronizer
    COMMAND $<TARGET_FILE:test_webdavsynchronizer> -platform minimal
)

if(OPENTODOLIST_NEXTCLOUD_TEST_URL)
    message("Running WebDAV Synchronizer tests against NextCloud")
    target_compile_definitions(
        test_webdavsynchronizer
        PRIVATE
            -DOPENTODOLIST_NEXTCLOUD_TEST_URL="${OPENTODOLIST_NEXTCLOUD_TEST_URL}"
    )
endif()

if(OPENTODOLIST_OWNCLOUD_TEST_URL)
    message("Running WebDAV Synchronizer tests against ownCloud")
    target_compile_definitions(
        test_webdavsynchronizer
        PRIVATE
            -DOPENTODOLIST_OWNCLOUD_TEST_URL="${OPENTODOLIST_OWNCLOUD_TEST_URL}"
    )
endif()

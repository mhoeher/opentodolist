# Test server URLs to be used by unit tests:
set(OPENTODOLIST_NEXTCLOUD_TEST_URL ""
    CACHE STRING "URL of a NextCloud test instance.")
set(OPENTODOLIST_OWNCLOUD_TEST_URL ""
    CACHE STRING "URL of an ownCloud test instance.")


# Control of build specific features:
option(OPENTODOLIST_WITH_UPDATE_SERVICE OFF
    "Enable built in update checks."
)
option(OPENTODOLIST_WITH_APPIMAGE_EXTRAS OFF
    "Enable features specific to the AppImage build."
)

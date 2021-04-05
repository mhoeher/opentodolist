# Test server URLs to be used by unit tests:
set(OPENTODOLIST_NEXTCLOUD_TEST_URL ""
    CACHE STRING "URL of a NextCloud test instance.")
set(OPENTODOLIST_OWNCLOUD_TEST_URL ""
    CACHE STRING "URL of an ownCloud test instance.")


# Control of build specific features:
option(OPENTODOLIST_WITH_UPDATE_SERVICE
    "Enable built in update checks."
    OFF
)
option(OPENTODOLIST_WITH_APPIMAGE_EXTRAS
    "Enable features specific to the AppImage build."
    OFF
)
option(OPENTODOLIST_WITH_KNOTIFICATIONS
    "Use KNotifications instead of Qt's built-in system tray implementation"
    OFF
)

option(OPENTODOLIST_USE_SYSTEM_LIBRARIES
    "The default mode used for deciding whether to build against system libraries."
    OFF
)

option(OPENTODOLIST_USE_SYSTEM_QTKEYCHAIN
    "Build against the system QtKeychain library."
    ${OPENTODOLIST_USE_SYSTEM_LIBRARIES}
)
option(OPENTODOLIST_USE_SYSTEM_KF_SYNTAX_HIGHLIGHTING
    "Build against the system KDE Syntax Highlighting library."
    ${OPENTODOLIST_USE_SYSTEM_LIBRARIES}
)
option(OPENTODOLIST_USE_SYSTEM_QLMDB
    "Build against the system QLMDB library."
     ${OPENTODOLIST_USE_SYSTEM_LIBRARIES}
)
option(OPENTODOLIST_USE_SYSTEM_SINGLEAPPLICATION
    "Build against the system SingleApplication library."
     ${OPENTODOLIST_USE_SYSTEM_LIBRARIES}
)
option(OPENTODOLIST_USE_SYSTEM_SYNQCLIENT
    "Build against the system SynqClient library."
     ${OPENTODOLIST_USE_SYSTEM_LIBRARIES}
)
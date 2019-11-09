include(../../config.pri)
setupTest(webdavsynchronizer)

include(../../lib/lib.pri)

SOURCES +=     test_webdavsynchronizer.cpp


!equals(NEXTCLOUD_URL,"") {
    DEFINES += OPENTODOLIST_NEXTCLOUD_TEST_URL=\\\"$$NEXTCLOUD_URL\\\"
}
!equals(OWNCLOUD_URL,"") {
    DEFINES += OPENTODOLIST_OWNCLOUD_TEST_URL=\\\"$$OWNCLOUD_URL\\\"
}

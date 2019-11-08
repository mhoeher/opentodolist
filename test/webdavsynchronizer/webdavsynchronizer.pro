include(../../config.pri)
setupTest(webdavsynchronizer)

include(../../lib/lib.pri)

SOURCES +=     test_webdavsynchronizer.cpp


!equals(NEXTCLOUD_URL,"") {
    DEFINES += NEXTCLOUD_URL=\\\"$$NEXTCLOUD_URL\\\"
}
!equals(OWNCLOUD_URL,"") {
    DEFINES += OWNCLOUD_URL=\\\"$$OWNCLOUD_URL\\\"
}

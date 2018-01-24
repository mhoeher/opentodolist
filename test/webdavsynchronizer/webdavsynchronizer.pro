include(../../config.pri)
setupTest(webdavsynchronizer)

include(../../lib/lib.pri)

SOURCES +=     test_webdavsynchronizer.cpp

with_nextcloud_tests {
    DEFINES += NEXTCLOUD_URL=\\\"$$NEXTCLOUD_URL\\\"
    DEFINES += OWNCLOUD_URL=\\\"$$OWNCLOUD_URL\\\"
}

include(../../config.pri)
setupTest(webdavsynchronizer)

include(../../lib/lib.pri)

SOURCES +=     test_webdavsynchronizer.cpp

with_nextcloud_tests {
    DEFINES += NEXTCLOUD_URL=\\\"$$NEXTCLOUD_URL\\\"
    DEFINES += NEXTCLOUD_USER=\\\"$$NEXTCLOUD_USER\\\"
    DEFINES += NEXTCLOUD_PASSWORD=\\\"$$NEXTCLOUD_PASSWORD\\\"
}

include(../../config.pri)
setupTest(webdavsynchronizer)

include(../../lib/lib.pri)

SOURCES +=     test_webdavsynchronizer.cpp

message("Checking for available WebDav Sync Test Backends...")
isEmpty($$NEXTCLOUD_URL) {
    message("Activating NextCloud sync tests")
    DEFINES += NEXTCLOUD_URL=\\\"$$NEXTCLOUD_URL\\\"
    DEFINES += NEXTCLOUD_USER=\\\"$$NEXTCLOUD_USER\\\"
    DEFINES += NEXTCLOUD_PASSWORD=\\\"$$NEXTCLOUD_PASSWORD\\\"
}

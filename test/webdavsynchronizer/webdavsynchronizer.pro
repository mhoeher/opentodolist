include(../../config.pri)
setupTest(webdavsynchronizer)

include(../../lib/lib.pri)

SOURCES +=     test_webdavsynchronizer.cpp

with_nextcloud_tests {
    !isEmpty($$NEXTCLOUD_URL,) {
        DEFINES += NEXTCLOUD_URL=\\\"$$NEXTCLOUD_URL\\\"
    }
    !isEmpty($$OWNCLOUD_URL,) {
        DEFINES += OWNCLOUD_URL=\\\"$$OWNCLOUD_URL\\\"
    }
}

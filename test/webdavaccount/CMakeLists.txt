find_package(Qt5Test REQUIRED)
add_executable(
    test_webdavaccount

    test_webdavaccount.cpp
)

target_link_libraries(
    test_webdavaccount

    # Internal libraries:
    opentodolist-common

    # Qt Libraries
    Qt5::Test
)

add_test(
    NAME webdavaccount
    COMMAND $<TARGET_FILE:test_webdavaccount> -platform minimal
)

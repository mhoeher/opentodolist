# On Android, we use our own implementation
if(NOT ANDROID)
    add_subdirectory(qtkeychain)
endif()

add_subdirectory(qlmdb)
add_subdirectory(synqclient)

if(ANDROID)
    add_subdirectory(android-openssl)
endif()

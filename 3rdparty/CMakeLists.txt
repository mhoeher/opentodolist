# On Android, we use our own implementation
if(NOT ANDROID)
    if(NOT OPENTODOLIST_USE_SYSTEM_QTKEYCHAIN)
        add_subdirectory(qtkeychain)
    endif()
endif()

if(NOT OPENTODOLIST_USE_SYSTEM_QLMDB)
    add_subdirectory(qlmdb)
endif()

if(NOT OPENTODOLIST_USE_SYSTEM_SYNQCLIENT)
    add_subdirectory(synqclient)
endif()

if(ANDROID)
    add_subdirectory(android-openssl)
endif()

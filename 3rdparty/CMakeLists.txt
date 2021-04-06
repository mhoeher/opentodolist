# On Android, we use our own implementation
if(NOT ANDROID)
    if(NOT OPENTODOLIST_USE_SYSTEM_QTKEYCHAIN)
        set(BUILD_TRANSLATIONS ON)
        add_subdirectory(qtkeychain)
        unset(BUILD_TRANSLATIONS)
    endif()
endif()

if(NOT OPENTODOLIST_USE_SYSTEM_QLMDB)
    add_subdirectory(qlmdb)
endif()

if(NOT OPENTODOLIST_USE_SYSTEM_SYNQCLIENT)
    add_subdirectory(synqclient)
endif()

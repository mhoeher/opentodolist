if(NOT OPENTODOLIST_FLATPAK)
    # Only include this if we build a Flatpak
    return()
endif()

include(GNUInstallDirs)

# Include FontConfig config which uses the Emoji One font from the
# KDE Flatpak SDK.
install(
    FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Flatpak/90-otl-color-emoji.conf
    DESTINATION
        ${CMAKE_INSTALL_SYSCONFDIR}/fonts/conf.d/
)

set(LINUXDEPLOYQT_URL "https://github.com/probonopd/linuxdeployqt/releases/download/1/linuxdeployqt-1-x86_64.AppImage")
set(LINUXDEPLOYQT_TOOL ${CMAKE_CURRENT_BINARY_DIR}/linuxdeployqt-1-x86_64.AppImage)

set(APPIMAGE ${CMAKE_CURRENT_BINARY_DIR}/app/OpenTodoList.AppImage)
set(APPIMAGE_DIR ${CMAKE_CURRENT_BINARY_DIR}/OpenTodoList-${OPENTODOLIST_VERSION})

add_custom_command(
    OUTPUT
        ${LINUXDEPLOYQT_TOOL}
    COMMAND
        wget -c ${LINUXDEPLOYQT_URL}
    COMMAND
        chmod a+x ${LINUXDEPLOYQT_TOOL}
)

add_custom_target(
    appimage

    COMMAND
        ${CMAKE_COMMAND} -E remove_directory ${APPIMAGE_DIR}
    COMMAND
        ${CMAKE_COMMAND} -E make_directory ${APPIMAGE_DIR}
    COMMAND
        ${CMAKE_COMMAND} -E copy $<TARGET_FILE:OpenTodoList> ${APPIMAGE_DIR}
    COMMAND
        ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/templates/appimage/default.desktop
            ${APPIMAGE_DIR}
    COMMAND
        ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/app/res/OpenTodoList80.png
            ${APPIMAGE_DIR}/OpenTodoList.png
    COMMAND
        ${CMAKE_COMMAND} -E env PATH=${QT_INSTALL_PREFIX}/bin:$ENV{PATH} ${LINUXDEPLOYQT_TOOL}
            ${APPIMAGE_DIR}/OpenTodoList -appimage
            -qmldir=${CMAKE_SOURCE_DIR}/app/
            -always-overwrite -bundle-non-qt-libs -verbose=2 || true
    COMMAND
        ${CMAKE_COMMAND} -E env PATH=${QT_INSTALL_PREFIX}/bin:$ENV{PATH} ${LINUXDEPLOYQT_TOOL}
            ${APPIMAGE_DIR}/OpenTodoList -appimage
            -qmldir=${CMAKE_SOURCE_DIR}/app/
            -always-overwrite -bundle-non-qt-libs -verbose=2 || true
    WORKING_DIRECTORY
        ${CMAKE_CURRENT_BINARY_DIR}
)

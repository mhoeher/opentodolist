find_package(Qt5 COMPONENTS REQUIRED Core)
get_target_property(QMAKE_EXECUTABLE Qt5::qmake IMPORTED_LOCATION)

set(
    LINUXDEPLOYQT_URL
    https://github.com/probonopd/linuxdeployqt/releases/download/continuous/linuxdeployqt-continuous-x86_64.AppImage
)
set(LINUXDEPLOYQT
    ./linuxdeployqt-x86_64.AppImage
)

add_custom_target(
    appimage
    COMMAND
        curl -L -o ${LINUXDEPLOYQT} ${LINUXDEPLOYQT_URL}
    COMMAND
        chmod +x ${LINUXDEPLOYQT}
    COMMAND
        rm -rf AppImageBuild
    COMMAND
        mkdir -p AppImageBuild
    COMMAND
        cp app/OpenTodoList AppImageBuild/
    COMMAND
        cp ${CMAKE_CURRENT_SOURCE_DIR}/templates/appimage/default.desktop AppImageBuild/
    COMMAND
        mkdir -p AppImageBuild/usr/share/icons/hicolor/
    COMMAND
        cp -r ${CMAKE_CURRENT_SOURCE_DIR}/app/icons/* AppImageBuild/usr/share/icons/hicolor/
    COMMAND
        cp -r ${CMAKE_CURRENT_SOURCE_DIR}/app/icons/hicolor/64x64/apps/net.rpdev.OpenTodoList.png AppImageBuild/
    COMMAND
        ${LINUXDEPLOYQT} --appimage-extract
    COMMAND
        ./squashfs-root/AppRun AppImageBuild/OpenTodoList
            -qmldir=${CMAKE_CURRENT_SOURCE_DIR}/app -qmake=${QMAKE_EXECUTABLE}
            -appimage
)

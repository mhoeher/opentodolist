find_package(Qt5Core REQUIRED)
get_target_property(QMAKE_EXECUTABLE Qt5::qmake IMPORTED_LOCATION)

set(LINUXDEPLOYQT_URL
    https://github.com/probonopd/linuxdeployqt/releases/download/continuous/linuxdeployqt-continuous-x86_64.AppImage
)
set(LINUXDEPLOYQT
    ./linuxdeployqt-continuous-x86_64.AppImage
)
set(APPIMAGETOOL_URL
    https://github.com/AppImage/AppImageKit/releases/download/continuous/appimagetool-x86_64.AppImage
)
set(APPIMAGETOOL ./appimagetool-x86_64.AppImage)
set(DESKTOPINTEGRATION_URL
    https://raw.githubusercontent.com/AppImage/AppImageKit/master/desktopintegration
)

add_custom_target(
    appimage
    COMMAND
        wget -O ${LINUXDEPLOYQT} ${LINUXDEPLOYQT_URL}
    COMMAND
        chmod +x ${LINUXDEPLOYQT}
    COMMAND
        wget -O ${APPIMAGETOOL} ${APPIMAGETOOL_URL}
    COMMAND
        chmod +x ${APPIMAGETOOL}
    COMMAND
        curl -o OpenTodoList.wrapper ${DESKTOPINTEGRATION_URL}
    COMMAND
        chmod +x OpenTodoList.wrapper
    COMMAND
        rm -rf AppImageBuild
    COMMAND
        mkdir -p AppImageBuild
    COMMAND
        cp OpenTodoList.wrapper AppImageBuild/OpenTodoList.wrapper
    COMMAND
        cp app/OpenTodoList AppImageBuild/
    COMMAND
        cp ${CMAKE_CURRENT_SOURCE_DIR}/templates/appimage/default.desktop AppImageBuild/
    COMMAND
        mkdir -p AppImageBuild/usr/share/icons/hicolor/
    COMMAND
        cp -r ${CMAKE_CURRENT_SOURCE_DIR}/templates/icons/* AppImageBuild/usr/share/icons/hicolor/
    COMMAND
        cp -r ${CMAKE_CURRENT_SOURCE_DIR}/templates/icons/64x64/apps/OpenTodoList.png AppImageBuild/
    COMMAND
        ${LINUXDEPLOYQT} AppImageBuild/OpenTodoList
            -qmldir=${CMAKE_CURRENT_SOURCE_DIR}/app -qmake=${QMAKE_EXECUTABLE}
    COMMAND
        rm AppImageBuild/AppRun || /bin/true
    COMMAND
        ln -s OpenTodoList.wrapper AppImageBuild/AppRun
    COMMAND
        ${APPIMAGETOOL} AppImageBuild OpenTodoList-x86_64.AppImage
)

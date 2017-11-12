LINUXDEPLOYQT_URL = https://github.com/probonopd/linuxdeployqt/releases/download/continuous/linuxdeployqt-continuous-x86_64.AppImage
LINUXDEPLOYQT = ./linuxdeployqt-continuous-x86_64.AppImage
APPIMAGETOOL_URL = https://github.com/AppImage/AppImageKit/releases/download/continuous/appimagetool-x86_64.AppImage
APPIMAGETOOL = ./appimagetool-x86_64.AppImage
DESKTOPINTEGRATION_URL = https://raw.githubusercontent.com/AppImage/AppImageKit/master/desktopintegration

linuxdeployqt.target = $$LINUXDEPLOYQT
linuxdeployqt.commands = \
    wget -O $$LINUXDEPLOYQT $$LINUXDEPLOYQT_URL && \
    chmod +x $$LINUXDEPLOYQT

appimagetool.target = $$APPIMAGETOOL
appimagetool.commands = \
    wget -O $$APPIMAGETOOL $$APPIMAGETOOL_URL && \
    chmod +x $$APPIMAGETOOL

appimage.target = OpenTodoList-x86_64.AppImage
appimage.depends = linuxdeployqt appimagetool
appimage.commands = \
    rm -rf AppImageBuild && \
    mkdir -p AppImageBuild && \
    cp app/OpenTodoList AppImageBuild/ && \
    curl -o AppImageBuild/OpenTodoList.wrapper $$DESKTOPINTEGRATION_URL && \
    chmod +x AppImageBuild/OpenTodoList.wrapper && \
    cp $$PWD/templates/appimage/default.desktop AppImageBuild/ && \
    mkdir -p AppImageBuild/usr/share/icons/hicolor/ && \
    cp -r $$PWD/templates/icons/* AppImageBuild/usr/share/icons/hicolor/ && \
    cp -r $$PWD/templates/icons/64x64/apps/OpenTodoList.png AppImageBuild/ && \
    $$LINUXDEPLOYQT AppImageBuild/OpenTodoList \
        -qmldir=$$PWD/app -qmake=$$QMAKE_QMAKE && \
    $$LINUXDEPLOYQT AppImageBuild/OpenTodoList \
        -qmldir=$$PWD/app -qmake=$$QMAKE_QMAKE && \
    (cd AppImageBuild && rm AppRun && ln -s OpenTodoList.wrapper AppRun) && \
    $$APPIMAGETOOL AppImageBuild OpenTodoList-x86_64.AppImage


QMAKE_EXTRA_TARGETS += linuxdeployqt appimagetool appimage

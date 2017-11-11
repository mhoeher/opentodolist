LINUXDEPLOYQT_URL = https://github.com/probonopd/linuxdeployqt/releases/download/continuous/linuxdeployqt-continuous-x86_64.AppImage
LINUXDEPLOYQT = ./linuxdeployqt-continuous-x86_64.AppImage
DESKTOPINTEGRATION_URL = https://raw.githubusercontent.com/AppImage/AppImageKit/master/desktopintegration

linuxdeployqt.target = $$LINUXDEPLOYQT
linuxdeployqt.commands = \
    wget -O $$LINUXDEPLOYQT $$LINUXDEPLOYQT_URL && \
    chmod +x $$LINUXDEPLOYQT

appimage.target = OpenTodoList-x86_64.AppImage
appimage.depends = linuxdeployqt
appimage.commands = \
    rm -rf AppImageBuild && \
    mkdir -p AppImageBuild && \
    mkdir -p AppImageBuild/usr/bin && \
    cp app/OpenTodoList AppImageBuild/usr/bin && \
    curl -o AppImageBuild/usr/bin/OpenTodoList.wrapper $$DESKTOPINTEGRATION_URL && \
    chmod +x AppImageBuild/usr/bin/OpenTodoList.wrapper && \
    cp $$PWD/templates/appimage/default.desktop AppImageBuild/ && \
    mkdir -p AppImageBuild/usr/share/icons/default/ && \
    cp -r $$PWD/templates/icons/* AppImageBuild/usr/share/icons/default/ && \
    $$LINUXDEPLOYQT AppImageBuild/usr/bin/OpenTodoList \
        -appimage -qmldir=$$PWD/app -qmake=$$QMAKE_QMAKE && \
    $$LINUXDEPLOYQT AppImageBuild/OpenTodoList \
        -appimage -qmldir=$$PWD/app -qmake=$$QMAKE_QMAKE


QMAKE_EXTRA_TARGETS += linuxdeployqt appimage

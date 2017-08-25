LINUXDEPLOYQT_URL = https://github.com/probonopd/linuxdeployqt/releases/download/continuous/linuxdeployqt-continuous-x86_64.AppImage
LINUXDEPLOYQT = ./linuxdeployqt-continuous-x86_64.AppImage

linuxdeployqt.target = $$LINUXDEPLOYQT
linuxdeployqt.commands = \
    wget -O $$LINUXDEPLOYQT $$LINUXDEPLOYQT_URL && \
    chmod +x $$LINUXDEPLOYQT

appimage.target = OpenTodoList-x86_64.AppImage
appimage.depends = linuxdeployqt
appimage.commands = \
    rm -rf AppImageBuild && \
    mkdir -p AppImageBuild && \
    cp app/OpenTodoList AppImageBuild && \
    cp $$PWD/templates/appimage/default.desktop AppImageBuild/ && \
    cp $$PWD/app/res/OpenTodoList80.png AppImageBuild/OpenTodoList.png && \
    $$LINUXDEPLOYQT AppImageBuild/OpenTodoList \
        -appimage -qmldir=$$PWD/app -qmake=$$QMAKE_QMAKE && \
    $$LINUXDEPLOYQT AppImageBuild/OpenTodoList \
        -appimage -qmldir=$$PWD/app -qmake=$$QMAKE_QMAKE


QMAKE_EXTRA_TARGETS += linuxdeployqt appimage

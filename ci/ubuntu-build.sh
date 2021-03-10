#!/bin/bash

set -e

PREFIX_PATH=$QT_ROOT

which ninja-build || (apt-get update && apt-get install -y ninja-build)

if [ -n "$CI" ]; then
    curl -d install="true" -d adminlogin=admin -d adminpass=admin \
        http://nextcloud/index.php
    curl -d install="true" -d adminlogin=admin -d adminpass=admin \
            http://owncloud/index.php
    QMAKE_EXTRA_FLAGS="
        NEXTCLOUD_URL=http://admin:admin@nextcloud/
        OWNCLOUD_URL=http://admin:admin@owncloud/
    "
    CMAKE_EXTRA_FLAGS="
        -DOPENTODOLIST_NEXTCLOUD_TEST_URL=http://admin:admin@nextcloud/
        -DOPENTODOLIST_OWNCLOUD_TEST_URL=http://admin:admin@owncloud/
    "
fi

which cmake || (apt-get update && apt-get install -y cmake)

[ -f /usr/lib/x86_64-linux-gnu/libssl.so ] || \
    (apt-get update && apt-get install -y libssl-dev)

desktop-file-validate templates/appimage/default.desktop

source $PREFIX_PATH/bin/qt511-env.sh || true
export QT_QPA_PLATFORM=minimal

mkdir -p build-ubuntu
cd build-ubuntu

cmake \
    -GNinja \
    -DCMAKE_PREFIX_PATH=$QT_ROOT \
    -DCMAKE_BUILD_TYPE=Release \
    -DOPENTODOLIST_WITH_APPIMAGE_EXTRAS=ON \
    -DOPENTODOLIST_WITH_UPDATE_SERVICE=ON \
    -DCMAKE_CXX_COMPILER_LAUNCHER=ccache \
    -DCMAKE_C_COMPILER_LAUNCHER=ccache \
    -DCMAKE_INSTALL_PREFIX=/usr \
    $CMAKE_EXTRA_FLAGS \
    ..
cmake --build .
cmake --build . --target test
rm -rf AppImageBuild
DESTDIR=$PWD/AppImageBuild cmake --build . --target install

# Remove unnecessary stuff:
rm -rf AppImageBuild/usr/{include,lib,mkspecs}

# Add symlinks to app root, so they are found by linuxdeployqt:
ln -s "./usr/share/applications/net.rpdev.OpenTodoList.desktop" AppImageBuild/default.desktop
ln -s "./usr/share/icons/hicolor/256x256/apps/net.rpdev.OpenTodoList.png" AppImageBuild/

export VERSION=$(git describe --tags)

LINUXDEPLOYQT="./linuxdeployqt-x86_64.AppImage"
if [ ! -f "$LINUXDEPLOYQT" ]; then
    curl -L -o "$LINUXDEPLOYQT" \
        https://github.com/probonopd/linuxdeployqt/releases/download/continuous/linuxdeployqt-continuous-x86_64.AppImage
    chmod +x "$LINUXDEPLOYQT"
fi

"$LINUXDEPLOYQT" --appimage-extract
./squashfs-root/AppRun AppImageBuild/usr/bin/OpenTodoList \
    -qmldir=../app -qmake=$QT_ROOT/bin/qmake \
    -appimage \
    -extra-plugins=platforms/libqwayland-generic.so,platforminputcontexts,wayland-graphics-integration-client

#!/bin/bash

set -e

if [ ! -d "$QT_INSTALL_ROOT" ]; then
    if [ -d "$HOME/Qt" ]; then
        QT_INSTALL_ROOT="$HOME/Qt"
    else
        echo "The variable QT_INSTALL_ROOT is not set"
        exit 1
    fi
fi
echo "Using Qt installation in $QT_INSTALL_ROOT"

if [ -z "$QT_VERSION" ]; then
    QT_VERSION=$(ls "$QT_INSTALL_ROOT" | grep -E '[0-9]+\.[0-9]+\.[0-9]+' | sort -V | tail -n1)
fi
echo "Using Qt $QT_VERSION"

if [ -n "$CI" ]; then
    curl -d install="true" -d adminlogin=admin -d adminpass=admin \
        http://nextcloud/index.php
    curl -d install="true" -d adminlogin=admin -d adminpass=admin \
            http://owncloud/index.php
    CMAKE_EXTRA_FLAGS="
        -DOPENTODOLIST_NEXTCLOUD_TEST_URL=http://admin:admin@nextcloud/
        -DOPENTODOLIST_OWNCLOUD_TEST_URL=http://admin:admin@owncloud/
    "
fi

export QT_QPA_PLATFORM=minimal

mkdir -p build-ubuntu
cd build-ubuntu

cmake \
    -GNinja \
    -DCMAKE_PREFIX_PATH=$QT_INSTALL_ROOT/$QT_VERSION/gcc_64 \
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

desktop-file-validate AppImageBuild/default.desktop

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
    -extra-plugins=platforminputcontexts

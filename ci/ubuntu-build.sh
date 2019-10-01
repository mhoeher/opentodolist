#!/bin/bash

set -e

PREFIX_PATH=$QT_ROOT

which ninja-build || (apt-get update && apt-get install -y ninja-build)

if [ -n "$CI" ]; then
    curl -d install="true" -d adminlogin=admin -d adminpass=admin \
        http://nextcloud/index.php
    curl -d install="true" -d adminlogin=admin -d adminpass=admin \
            http://owncloud/index.php
    CMAKE_EXTRA_FLAGS="
        -DOPENTODOLIST_NEXTCLOUD_TEST_URL=http://admin:admin@nextcloud/
        -DOPENTODOLIST_OWNCLOUD_TEST_URL=http://admin:admin@owncloud/
    "

    # Install ECM:
    pushd 3rdparty/KDE/extra-cmake-modules
    mkdir -p build-ubuntu
    cd build-ubuntu
    cmake -DCMAKE_PREFIX_PATH=$PREFIX_PATH -GNinja ..
    cmake --build .
    cmake --build . --target install
    popd

    # Install KDE syntax highlighting
    pushd 3rdparty/KDE/syntax-highlighting/
    mkdir -p build-ubuntu
    cd build-ubuntu
    cmake -DCMAKE_PREFIX_PATH=$PREFIX_PATH -GNinja ..
    cmake --build .
    cmake --build . --target install
    popd
fi

which cmake || (apt-get update && apt-get install -y cmake)

[ -f /usr/lib/x86_64-linux-gnu/libssl.so ] || (apt-get update && apt-get install -y libssl-dev)

desktop-file-validate templates/appimage/default.desktop

source $PREFIX_PATH/bin/qt511-env.sh || true
export QT_QPA_PLATFORM=minimal

mkdir -p build-ubuntu
cd build-ubuntu

cmake \
    -DCMAKE_PREFIX_PATH=$PREFIX_PATH \
    -GNinja \
    $CMAKE_EXTRA_FLAGS \
    -DCMAKE_BUILD_TYPE=Release \
    -DOPENTODOLIST_WITH_UPDATE_SERVICE=ON \
    -DOPENTODOLIST_WITH_APPIMAGE_EXTRAS=ON \
    -DQTKEYCHAIN_STATIC=ON \
    ..
cmake --build . -- opentodolist-translations
cmake --build . --target all -- -j4
cmake --build . --target test
cmake --build . --target appimage

make OpenTodoList-x86_64.AppImage

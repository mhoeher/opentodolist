#!/bin/bash

set -e

SCRIPT_DIR="$(cd $(dirname "$0") && pwd)"
cd "$SCRIPT_DIR"
cd ..

if [ -n "$CI" ]; then
    dnf install -y \
        make cmake ninja-build gcc g++ curl ccache \
        qt6-{qtbase,qttools,qtdeclarative,qtremoteobjects,qtquickcontrols2,qtnetworkauth}-devel \
        qt6-qtbase-{mysql,odbc,postgresql} \
        qt6-linguist \
        libsecret-devel \
        perl

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

mkdir -p fedora-build-cmake
cd fedora-build-cmake

if [ -n "$SYSTEM_LIBS" ]; then
    for project in KDE/extra-cmake-modules KDE/syntax-highlighting qlmdb synqclient qtkeychain; do
        cmake \
            -S ../3rdparty/$project \
            -B $project-build \
            -GNinja \
            -DCMAKE_BUILD_TYPE=Release \
            -DCMAKE_INSTALL_PREFIX=$PWD/_ \
            -DCMAKE_PREFIX_PATH=$PWD/_ \
            -DBUILD_WITH_QT6=ON \
            -DQLMDB_WITHOUT_TESTS=ON \
            -DSYNQCLIENT_WITHOUT_TESTS=ON
        cmake --build $project-build
        cmake --install $project-build
        # If we build in the CIs, remove the source folder to make sure the app build
        # does not exidentally pick the in-source version up
        if [ -n "$CI" ]; then
            mv ../3rdparty/$project ../3rdparty/$project-bak
        fi
    done
    CMAKE_EXTRA_FLAGS="$CMAKE_EXTRA_FLAGS -DOPENTODOLIST_USE_SYSTEM_LIBRARIES=ON -DCMAKE_PREFIX_PATH=$PWD/_"
fi


cmake \
    -GNinja \
    -DCMAKE_BUILD_TYPE=RelWithDebInfo \
    -DOPENTODOLIST_WITH_UPDATE_SERVICE=ON \
    -DCMAKE_INSTALL_PREFIX=/usr \
    ${CMAKE_EXTRA_FLAGS} \
    ..
cmake --build .
cmake --build . --target test

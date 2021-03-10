#!/bin/bash

set -e

SCRIPT_DIR="$(cd $(dirname "$0") && pwd)"
cd "$SCRIPT_DIR"
cd ..

if [ -n "$CI" ]; then
    dnf install -y \
        make cmake ninja-build gcc g++ curl ccache \
        qt5-{qtbase,qtdeclarative,qtremoteobjects,qtquickcontrols2}-devel \
        qt5-linguist

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

export QT_QPA_PLATFORM=minimal


if [ "$BUILD_SYSTEM" == "qmake" ]; then
    mkdir -p fedora-build-qmake
    cd fedora-build-qmake
    qmake-qt5 \
        CONFIG+=release \
        CONFIG+=with_update_service \
        CONFIG+=qlmdb_with_static_libs \
        CONFIG+=synqclient_with_static_libs \
        CONFIG+=ccache \
        INSTALL_PREFIX=/usr \
        $QMAKE_EXTRA_FLAGS \
        ..
    make -j4
    make check
else
    mkdir -p fedora-build-cmake
    cd fedora-build-cmake
    cmake \
        -GNinja \
        -DCMAKE_BUILD_TYPE=RelWithDebInfo \
        -DDOPENTODOLIST_WITH_UPDATE_SERVICE=ON \
        -DCMAKE_INSTALL_PREFIX=/usr \
        ${CMAKE_EXTRA_FLAGS} \
        ..
    cmake --build .
    cmake --build . --target test
fi

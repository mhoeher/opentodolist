#!/bin/bash

set -e

SCRIPT_DIR="$(cd $(dirname "$0") && pwd)"
cd "$SCRIPT_DIR"
cd ..

if [ -n "$CI" ]; then
    dnf install -y \
        make cmake ninja-build gcc g++ curl ccache \
        qt5-{qtbase,qtdeclarative,qtremoteobjects,qtquickcontrols2}-devel \
        qt5-linguist \
        libsecret-devel

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

    if [ -n "$SYSTEM_LIBS" ]; then
        if [ -n "$CI" ]; then
            dnf install -y \
              qtkeychain-qt5-devel \
              kf5-syntax-highlighting-devel
        fi

        for project in qlmdb synqclient; do
            mkdir -p ${project}-build
            cd ${project}-build
            cmake \
                -GNinja \
                -DCMAKE_BUILD_TYPE=RelWithDebInfo \
                -DCMAKE_INSTALL_PREFIX=$PWD/../_ \
                ../../3rdparty/${project}
            cmake --build .
            cmake --install .
            cd ..
        done
        CMAKE_EXTRA_FLAGS="$CMAKE_EXTRA_FLAGS -DOPENTODOLIST_USE_SYSTEM_LIBRARIES=ON -DCMAKE_PREFIX_PATH=$PWD/_"
    fi

    # If we build against system libs, move included libraries away, so we
    # cannot accidentally include them.
    if [ -n "$CI" ]; then
        if [ -n "$SYSTEM_LIBS" ]; then
            pushd ../3rdparty
            for dir in *; do
                if [ -d "$dir" ]; then
                    if [ "$dir" != "simplecrypt" -a "$dir" != "SingleApplication" ]; then
                        mv "$dir" "${dir}~"
                    fi
                fi
            done
            popd
        fi
    fi

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

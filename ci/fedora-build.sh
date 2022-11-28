#!/bin/bash

set -e

SCRIPT_DIR="$(cd $(dirname "$0") && pwd)"
cd "$SCRIPT_DIR"
cd ..

if [ -n "$CI" ]; then
    dnf install -y \
        make cmake ninja-build gcc g++ curl ccache \
        qt6-{qtbase,qttools,qtdeclarative,qtremoteobjects,qtquickcontrols2,qtnetworkauth}-devel \
        qt6-linguist \
        libsecret-devel

    # curl -d install="true" -d adminlogin=admin -d adminpass=admin \
    #     http://nextcloud/index.php
    # curl -d install="true" -d adminlogin=admin -d adminpass=admin \
    #         http://owncloud/index.php
    # CMAKE_EXTRA_FLAGS="
    #     -DOPENTODOLIST_NEXTCLOUD_TEST_URL=http://admin:admin@nextcloud/
    #     -DOPENTODOLIST_OWNCLOUD_TEST_URL=http://admin:admin@owncloud/
    # "
fi

export QT_QPA_PLATFORM=minimal

mkdir -p fedora-build-cmake
cd fedora-build-cmake

if [ -n "$SYSTEM_LIBS" ]; then
    for project in qlmdb synqclient qtkeychain KDE/extra-cmake-modules KDE/syntax-highlighting; do
        cmake \
            -S ../3rdparty/$project \
            -B $project-build \
            -DCMAKE_BUILD_TYPE=RelWithDebInfo \
            -DCMAKE_INSTALL_PREFIX=$PWD/_ \
            -DCMAKE_PREFIX_PATH=$PWD/_ \
            -DBUILD_WITH_QT6=ON \
            -DQLMDB_WITHOUT_TESTS=ON \
            -DSYNQCLIENT_WITHOUT_TESTS=ON
        cmake --build $project-build
        cmake --install $project-build
    done
    CMAKE_EXTRA_FLAGS="$CMAKE_EXTRA_FLAGS -DOPENTODOLIST_USE_SYSTEM_LIBRARIES=ON -DCMAKE_PREFIX_PATH=$PWD/_"
fi

# If we build against system libs, move included libraries away, so we
# cannot accidentally include them.
# if [ -n "$CI" ]; then
#     if [ -n "$SYSTEM_LIBS" ]; then
#         pushd ../3rdparty
#         for dir in *; do
#             if [ -d "$dir" ]; then
#                 if [ "$dir" != "simplecrypt" -a "$dir" != "SingleApplication" -a "$dir" != "ral-json" ]; then
#                     mv "$dir" "${dir}~"
#                 fi
#             fi
#         done
#         popd
#     fi
# fi

cmake \
    -GNinja \
    -DCMAKE_BUILD_TYPE=RelWithDebInfo \
    -DDOPENTODOLIST_WITH_UPDATE_SERVICE=ON \
    -DCMAKE_INSTALL_PREFIX=/usr \
    ${CMAKE_EXTRA_FLAGS} \
    ..
cmake --build .
cmake --build . --target test

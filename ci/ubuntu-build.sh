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
fi

which cmake || (apt-get update && apt-get install -y cmake)

[ -f /usr/lib/x86_64-linux-gnu/libssl.so ] || \
    (apt-get update && apt-get install -y libssl-dev)

desktop-file-validate templates/appimage/default.desktop

source $PREFIX_PATH/bin/qt511-env.sh || true
export QT_QPA_PLATFORM=minimal

mkdir -p build-ubuntu
cd build-ubuntu

$QT_ROOT/bin/qmake \
    CONFIG+=release \
    CONFIG+=with_appimage_extras \
    CONFIG+=with_update_service \
    CONFIG+=qlmdb_with_static_libs \
    CONFIG+=ccache \
    INSTALL_PREFIX=/usr \
    $QMAKE_EXTRA_FLAGS \
    ..
make -j4
make check
make appimage

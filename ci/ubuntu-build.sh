#!/bin/bash

set -e

if [ -n "$CI" ]; then
    curl -d install="true" -d adminlogin=admin -d adminpass=admin \
        http://nextcloud/index.php
    QMAKE_EXTRA_FLAGS="CONFIG+=with_nextcloud_tests"
fi

desktop-file-validate templates/appimage/default.desktop

source /opt/qt510/bin/qt510-env.sh || true
export QT_QPA_PLATFORM=minimal

mkdir build-ubuntu
cd build-ubuntu

qmake \
    CONFIG+=with_update_service \
    CONFIG+=with_appimage_extras \
    CONFIG+=RELEASE \
    $QMAKE_EXTRA_FLAGS \
    NEXTCLOUD_URL=http://nextcloud \
    NEXTCLOUD_USER=admin \
    NEXTCLOUD_PASSWORD=admin \
    ..
make -j4
make check
make OpenTodoList-x86_64.AppImage

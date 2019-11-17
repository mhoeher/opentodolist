#!/bin/bash

set -e

QMAKE=$QT_DIR_IOS/bin/qmake

mkdir -p build-ios
pushd build-ios
$QMAKE \
    -spec macx-ios-clang \
    CONFIG+=release \
    CONFIG+=iphoneos \
    CONFIG+=device \
    ..
make -j4
popd

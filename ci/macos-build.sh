#!/bin/bash

set -e

mkdir build-macos
cd build-macos

QTSDK=$QT_DIR/5.10.1/clang_64/

/Applications/CMake.app/Contents/bin/cmake \
    -DCMAKE_PREFIX_PATH=$QTSDK \
    -DCMAKE_BUILD_TYPE=Release \
    -DOPENTODOLIST_WITH_UPDATE_SERVICE=ON \
    -DQTKEYCHAIN_STATIC=ON \
    ..
make -j2
make test
$QTSDK/bin/macdeployqt app/OpenTodoList.app/ -dmg -qmldir=../app

#!/bin/bash
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
$QTSDK/bin/macdeployqt app/OpenTodoList.app/ -dmg -qmldir=../app

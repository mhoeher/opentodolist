#!/bin/bash

set -e

mkdir -p build-win32
cd build-win32

mingw32-cmake \
    -GNinja \
    -DCMAKE_BUILD_TYPE=Release \
    -DOPENTODOLIST_WITH_UPDATE_SERVICE=ON \
    -DUSE_CREDENTIAL_STORE=ON \
    -DQTKEYCHAIN_STATIC=ON \
    ..
cmake --build . -- opentodolist-translations
cmake --build . --target all

cd ..


mkdir -p deploy-win32/bin
cp \
    build-win32/app/OpenTodoList.exe \
    build-win32/3rdparty/qlmdb/qlmdb/libqlmdb.dll \
    /usr/i686-w64-mingw32/sys-root/mingw/bin/libcrypto-1_1.dll \
    /usr/i686-w64-mingw32/sys-root/mingw/bin/libssl-1_1.dll \
    /usr/i686-w64-mingw32/sys-root/mingw/bin/libEGL.dll \
    /usr/i686-w64-mingw32/sys-root/mingw/bin/libGLESv2.dll \
    \
    deploy-win32/bin/

wget https://gitlab.com/rpdev/xwindeployqt/raw/master/xwindeployqt
chmod +x xwindeployqt
./xwindeployqt \
    --driver fedora-mingw \
    --qml-dir app \
    --plugins platforms \
    --plugins imageformats \
    --plugins sqldrivers \
    --plugins bearer \
    --mingw-arch i686-w64-mingw32 \
    deploy-win32/bin/

cp templates/nsis/win32-installer.nsis deploy-win32/
cd deploy-win32
makensis win32-installer.nsis

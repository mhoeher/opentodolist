#!/bin/bash

set -e

mkdir -p build-win64
cd build-win64

mingw64-cmake \
    -GNinja \
    -DCMAKE_BUILD_TYPE=Release \
    -DOPENTODOLIST_WITH_UPDATE_SERVICE=ON \
    -DUSE_CREDENTIAL_STORE=ON \
    -DQTKEYCHAIN_STATIC=ON \
    ..
cmake --build . -- opentodolist-translations
cmake --build . --target all

cd ..


mkdir -p deploy-win64/bin
cp \
    build-win64/app/OpenTodoList.exe \
    build-win64/3rdparty/qlmdb/qlmdb/libqlmdb.dll \
    /usr/x86_64-w64-mingw32/sys-root/mingw/bin/libcrypto-1_1-x64.dll \
    /usr/x86_64-w64-mingw32/sys-root/mingw/bin/libssl-1_1-x64.dll \
    /usr/x86_64-w64-mingw32/sys-root/mingw/bin/libEGL.dll \
    /usr/x86_64-w64-mingw32/sys-root/mingw/bin/libGLESv2.dll \
    \
    deploy-win64/bin/

wget https://gitlab.com/rpdev/xwindeployqt/raw/master/xwindeployqt
chmod +x xwindeployqt
./xwindeployqt \
    --driver fedora-mingw \
    --qml-dir app \
    --plugins platforms \
    --plugins imageformats \
    --plugins sqldrivers \
    --plugins bearer \
    --mingw-arch x86_64-w64-mingw32 \
    deploy-win64/bin/

cp templates/nsis/win64-installer.nsis deploy-win64/
cd deploy-win64
makensis win64-installer.nsis

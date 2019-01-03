#!/bin/bash

set -e

export PATH=/opt/mxe-i686-shared/usr/bin:$PATH
export PATH=/opt/mxe-i686-shared/usr/i686-w64-mingw32.shared/qt5/bin:$PATH

mkdir -p build-win32
cd build-win32

i686-w64-mingw32.shared-cmake \
    -DCMAKE_BUILD_TYPE=Release \
    -DOPENTODOLIST_WITH_UPDATE_SERVICE=ON \
    -DUSE_CREDENTIAL_STORE=ON \
    -DQTKEYCHAIN_STATIC=ON \
    ..
i686-w64-mingw32.shared-cmake --build . -- opentodolist-translations
i686-w64-mingw32.shared-cmake --build . --target all -- -j4

cd ..
mkdir -p deploy-win32/bin
cp build-win32/app/OpenTodoList.exe deploy-win32/bin/
cp build-win32/3rdparty/qlmdb/qlmdb/libqlmdb.dll deploy-win32/bin/

wget https://gitlab.com/rpdev/xwindeployqt/raw/master/xwindeployqt
chmod +x xwindeployqt
./xwindeployqt --toolchain-root /opt/mxe-i686-shared/ \
    --toolchain-arch i686-w64-mingw32.shared \
    --qml-dir ./app \
    --plugins bearer \
    --plugins imageformats \
    --plugins sqldrivers \
    deploy-win32/bin/

cp templates/nsis/win32-installer.nsis deploy-win32/
cd deploy-win32
makensis win32-installer.nsis

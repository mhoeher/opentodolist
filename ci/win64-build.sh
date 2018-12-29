#!/bin/bash

set -e

export PATH=/opt/mxe-x64-shared/usr/bin:$PATH
export PATH=/opt/mxe-x64-shared/usr/x86_64-w64-mingw32.shared/qt5/bin:$PATH

mkdir build-win64
cd build-win64

x86_64-w64-mingw32.shared-cmake \
    -DCMAKE_BUILD_TYPE=Release \
    -DOPENTODOLIST_WITH_UPDATE_SERVICE=ON \
    -DUSE_CREDENTIAL_STORE=ON \
    -DQTKEYCHAIN_STATIC=ON \
    ..
x86_64-w64-mingw32.shared-cmake --build . -- opentodolist-translations
x86_64-w64-mingw32.shared-cmake --build . --target all -- -j4

cd ..
mkdir -p deploy-win64/bin
cp build-win64/app/OpenTodoList.exe deploy-win64/bin/

wget https://gitlab.com/rpdev/xwindeployqt/raw/master/xwindeployqt
chmod +x xwindeployqt
./xwindeployqt --toolchain-root /opt/mxe-x64-shared/ \
    --toolchain-arch x86_64-w64-mingw32.shared \
    --qml-dir ./app \
    --plugins bearer \
    --plugins imageformats \
    --plugins sqldrivers \
    deploy-win64/bin/

cp templates/nsis/win64-installer.nsis deploy-win64/
cd deploy-win64
makensis win64-installer.nsis

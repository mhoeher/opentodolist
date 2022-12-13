#!/bin/bash

set -e

if [ -n "$CI" ]; then
    if [ ! -f /bin/find ]; then
        dnf install -y --nogpgcheck findutils ccache
    fi
fi

VERSION=$(git describe --tags)

# Ignore platform checks - needed to get the indexer build
export KF_IGNORE_PLATFORM_CHECK="ON"

if [ "$TARGET" == win64 ]; then
    BUILD_DIR=build-win64
    DEPLOY_DIR=deploy-win64
    MINGW_DIR=x86_64-w64-mingw32
    CMAKE=mingw64-cmake
    INSTALLER_FILE=win64-installer.nsis
    INSTALLER_OUTPUT_FILE=OpenTodoList-Windows-64bit.exe
    INSTALLER_OUTPUT_TARGET_FILE=OpenTodoList-${VERSION}-Windows-64bit.exe
    EXTRA_LIBS="\
        /usr/$MINGW_DIR/sys-root/mingw/bin/libcrypto-3-x64.dll \
        /usr/$MINGW_DIR/sys-root/mingw/bin/libssl-3-x64.dll"
else
    BUILD_DIR=build-win32
    DEPLOY_DIR=deploy-win32
    MINGW_DIR=i686-w64-mingw32
    CMAKE=mingw32-cmake
    INSTALLER_FILE=win32-installer.nsis
    INSTALLER_OUTPUT_FILE=OpenTodoList-Windows-32bit.exe
    INSTALLER_OUTPUT_TARGET_FILE=OpenTodoList-${VERSION}-Windows-32bit.exe
    EXTRA_LIBS="\
        /usr/$MINGW_DIR/sys-root/mingw/bin/libcrypto-3.dll \
        /usr/$MINGW_DIR/sys-root/mingw/bin/libssl-3.dll"
fi

mkdir -p $BUILD_DIR
cd $BUILD_DIR

$CMAKE \
    -GNinja \
    -DCMAKE_BUILD_TYPE=Release \
    -DOPENTODOLIST_WITH_UPDATE_SERVICE=ON \
    -DCMAKE_CXX_COMPILER_LAUNCHER=ccache \
    -DCMAKE_C_COMPILER_LAUNCHER=ccache \
    -DCMAKE_INSTALL_PREFIX=/usr \
    -DQT_HOST_INSTALL_PREFIX=/usr \
    ..
cmake --build . --target OpenTodoList
rm -rf $PWD/_
DESTDIR=$PWD/_ cmake --build . --target install
mkdir -p "$PWD/../$DEPLOY_DIR"
cp -r _/usr/bin _/usr/share "$PWD/../$DEPLOY_DIR"

# Remove some extra files we don't need to deploy
rm -f \
    $PWD/../$DEPLOY_DIR/bin/*.dll.a \
    $PWD/../$DEPLOY_DIR/bin/*.prl

cd ..

cp \
    $EXTRA_LIBS \
    /usr/$MINGW_DIR/sys-root/mingw/bin/libEGL.dll \
    /usr/$MINGW_DIR/sys-root/mingw/bin/libGLESv2.dll \
    \
    $DEPLOY_DIR/bin/

if [ ! -f xwindeployqt ]; then
    wget https://gitlab.com/rpdev/xwindeployqt/raw/master/xwindeployqt
    chmod +x xwindeployqt
fi
./xwindeployqt \
    --driver fedora-mingw \
    --qml-dir app \
    --plugins platforms \
    --plugins imageformats \
    --plugins sqldrivers \
    --plugins tls \
    --mingw-arch $MINGW_DIR \
    --qt-version 6 \
    $DEPLOY_DIR/bin/

# Strip debug symbols to reduce file size:
find $DEPLOY_DIR/bin/ \
    -name '*.exe' -or -name '*.dll' \
    -exec mingw-strip {} \;

cp templates/nsis/$INSTALLER_FILE $DEPLOY_DIR/
cd $DEPLOY_DIR
makensis $INSTALLER_FILE

mv $INSTALLER_OUTPUT_FILE $INSTALLER_OUTPUT_TARGET_FILE

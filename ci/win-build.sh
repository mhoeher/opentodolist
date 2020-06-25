#!/bin/bash

set -e

if [ -n "$CI" ]; then
    if [ ! -f /bin/find ]; then
        dnf install -y --nogpgcheck findutils ccache
    fi
fi

if [ "$TARGET" == win64 ]; then
    BUILD_DIR=build-win64
    DEPLOY_DIR=deploy-win64
    MXE_DIR=x86_64-w64-mingw32
    CMAKE=mingw64-cmake
    QMAKE=mingw64-qmake-qt5
    INSTALLER_FILE=win64-installer.nsis
    EXTRA_LIBS="\
        /usr/$MXE_DIR/sys-root/mingw/bin/libcrypto-1_1-x64.dll \
        /usr/$MXE_DIR/sys-root/mingw/bin/libssl-1_1-x64.dll"
else
    BUILD_DIR=build-win32
    DEPLOY_DIR=deploy-win32
    MXE_DIR=i686-w64-mingw32
    CMAKE=mingw32-cmake
    QMAKE=mingw32-qmake-qt5
    INSTALLER_FILE=win32-installer.nsis
    EXTRA_LIBS="\
        /usr/$MXE_DIR/sys-root/mingw/bin/libcrypto-1_1.dll \
        /usr/$MXE_DIR/sys-root/mingw/bin/libssl-1_1.dll"
fi

mkdir -p $BUILD_DIR
cd $BUILD_DIR

$QMAKE \
    CONFIG+=release \
    CONFIG+=with_update_service \
    CONFIG+=ccache \
    CONFIG+=enable_cross_win_ccache \
    INSTALL_PREFIX=$PWD/../$DEPLOY_DIR \
    ..
make qmake_all
make -j4
make install

# Remove some extra files we don't need to deploy
rm -f \
    $PWD/../$DEPLOY_DIR/bin/*.dll.a \
    $PWD/../$DEPLOY_DIR/bin/*.prl

cd ..

cp \
    $EXTRA_LIBS \
    /usr/$MXE_DIR/sys-root/mingw/bin/libEGL.dll \
    /usr/$MXE_DIR/sys-root/mingw/bin/libGLESv2.dll \
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
    --plugins bearer \
    --mingw-arch $MXE_DIR \
    $DEPLOY_DIR/bin/

# Strip debug symbols to reduce file size:
find $DEPLOY_DIR/bin/ \
    -name '*.exe' -or -name '*.dll' \
    -exec mingw-strip {} \;

cp templates/nsis/$INSTALLER_FILE $DEPLOY_DIR/
cd $DEPLOY_DIR
makensis $INSTALLER_FILE

#!/bin/bash

set -e

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

if [ -n "$CI" ]; then
    # Install ECM
    pushd 3rdparty/KDE/extra-cmake-modules
    mkdir -p $BUILD_DIR
    cd $BUILD_DIR
    $CMAKE -GNinja ..
    cmake --build .
    cmake --build . --target install
    popd

    # Install KDE syntax highlighting
    pushd 3rdparty/KDE/syntax-highlighting
    mkdir -p $BUILD_DIR
    cd $BUILD_DIR
    $CMAKE -GNinja -DKF5_HOST_TOOLING=/usr ..
    cmake --build .
    cmake --build . --target install
    sed -i \
        -e 's#INTERFACE_INCLUDE_DIRECTORIES "/usr/i686-w64-mingw32/sys-root/mingw/include/KF5/KSyntaxHighlighting;/usr/i686-w64-mingw32/sys-root/mingw/include/KF5;/usr/i686-w64-mingw32/sys-root/mingw/include"#INTERFACE_INCLUDE_DIRECTORIES "/usr/i686-w64-mingw32/sys-root/mingw/include/KF5/KSyntaxHighlighting;/usr/i686-w64-mingw32/sys-root/mingw/include/KF5"#g' \
        -e 's#INTERFACE_INCLUDE_DIRECTORIES "/usr/x86_64-w64-mingw32/sys-root/mingw/include/KF5/KSyntaxHighlighting;/usr/x86_64-w64-mingw32/sys-root/mingw/include/KF5;/usr/x86_64-w64-mingw32/sys-root/mingw/include"#INTERFACE_INCLUDE_DIRECTORIES "/usr/x86_64-w64-mingw32/sys-root/mingw/include/KF5/KSyntaxHighlighting;/usr/x86_64-w64-mingw32/sys-root/mingw/include/KF5"#g' \
        /usr/$MXE_DIR/sys-root/mingw/lib/cmake/KF5SyntaxHighlighting/KF5SyntaxHighlightingTargets.cmake
    popd
fi

mkdir -p $BUILD_DIR
cd $BUILD_DIR

$QMAKE \
    CONFIG+=release \
    CONFIG+=with_update_service \
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

cp templates/nsis/$INSTALLER_FILE $DEPLOY_DIR/
cd $DEPLOY_DIR
makensis $INSTALLER_FILE

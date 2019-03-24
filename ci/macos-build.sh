#!/bin/bash

set -e

BUILD_DIR=$PWD/build-macos
PACKAGES_DIR=$BUILD_DIR/__packages

rm -rf $BUILD_DIR

if [ ! -d "$QT_DIR" ]; then
    echo "The variable QT_DIR is not set"
    exit 1
fi

QTSDK=$QT_DIR

if [ -n "$CI" ]; then
    # Install ECM:
    pushd 3rdparty/KDE/extra-cmake-modules
    mkdir -p build-macos
    cd build-macos
    cmake \
        -GNinja \
        -DCMAKE_PREFIX_PATH=$QTSDK \
        -DCMAKE_INSTALL_PREFIX=$PACKAGES_DIR \
        ..
    cmake --build .
    cmake --build . --target install
    popd

    # Install KDE syntax highlighting
    pushd 3rdparty/KDE/syntax-highlighting/
    mkdir -p build-macos
    cd build-macos
    cmake \
        -GNinja \
        -DCMAKE_PREFIX_PATH=$QTSDK \
        -DCMAKE_INSTALL_PREFIX=$PACKAGES_DIR \
        -Dt5_DIR=$QTSDK/lib/cmake \
        ..
    cmake --build .
    cmake --build . --target install
    popd
fi

mkdir -p $BUILD_DIR
cd $BUILD_DIR

cmake \
    -GNinja \
    -DCMAKE_PREFIX_PATH=$QTSDK\;$PACKAGES_DIR \
    -DCMAKE_BUILD_TYPE=Release \
    -DOPENTODOLIST_WITH_UPDATE_SERVICE=ON \
    -DQTKEYCHAIN_STATIC=ON \
    -DCMAKE_OSX_DEPLOYMENT_TARGET=10.10 \
    ..
cmake --build . --target opentodolist-translations
cmake --build . -- -j2
cmake --build . -- test
$QTSDK/bin/macdeployqt app/OpenTodoList.app/ -qmldir=../app

# Prepare a "beautified" folder:
cd app
mkdir dmg.in
rsync -a OpenTodoList.app dmg.in/
cp ../../templates/macos/DS_Store ./dmg.in/.DS_Store
cd dmg.in
ln -s /Applications ./Applications
cd ../

# Create DMG file:
hdiutil create -volname OpenTodoList -srcfolder ./dmg.in -ov -format UDZO OpenTodoList.dmg


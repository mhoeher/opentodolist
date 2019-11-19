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
    rm -rf build-macos
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
    rm -rf build-macos
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

    QMAKE_EXTRA_ARGS="\
        INCLUDEPATH+=$PACKAGES_DIR/include/ \
        LIBS+=-L$PACKAGES_DIR/lib"
    
    export QMAKEPATH=$QMAKEPATH:$PACKAGES_DIR
fi

#rm -rf $BUILD_DIR
mkdir -p $BUILD_DIR
cd $BUILD_DIR

$QTSDK/bin/qmake \
    CONFIG+=release \
    CONFIG+=with_update_service \
    CONFIG+=qlmdb_with_static_libs \
    $QMAKE_EXTRA_ARGS \
    ..
make -j4
make check
$QTSDK/bin/macdeployqt \
    app/OpenTodoList.app/ \
    -qmldir=../app \
    -appstore-compliant \
    -codesign="Apple Distribution: Martin Hoeher (786Z636JV9)"

# Make sure the app has been signed:
codesign -v app/OpenTodoList.app

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


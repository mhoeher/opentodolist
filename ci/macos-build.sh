#!/bin/bash

set -e

BUILD_DIR=$PWD/build-macos

rm -rf $BUILD_DIR

if [ ! -d "$QT_DIR" ]; then
    echo "The variable QT_DIR is not set"
    exit 1
fi

QTSDK=$QT_DIR

#rm -rf $BUILD_DIR
mkdir -p $BUILD_DIR
cd $BUILD_DIR

$QTSDK/bin/qmake \
    CONFIG+=release \
    CONFIG+=with_update_service \
    CONFIG+=qlmdb_with_static_libs \
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


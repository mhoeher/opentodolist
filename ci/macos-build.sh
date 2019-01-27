#!/bin/bash

set -e

rm -rf build-macos
mkdir build-macos
cd build-macos

if [ ! -d "$QT_DIR" ]; then
    echo "The variable QT_DIR is not set"
    exit 1
fi

QTSDK=$QT_DIR

/Applications/CMake.app/Contents/bin/cmake \
    -DCMAKE_PREFIX_PATH=$QTSDK \
    -DCMAKE_BUILD_TYPE=Release \
    -DOPENTODOLIST_WITH_UPDATE_SERVICE=ON \
    -DQTKEYCHAIN_STATIC=ON \
    -DCMAKE_OSX_DEPLOYMENT_TARGET=10.10 \
    ..
make opentodolist-translations
make -j2
make test
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


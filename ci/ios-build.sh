#!/bin/bash

set -e

QMAKE=$QT_DIR_IOS/bin/qmake

export XCODEBUILD_FLAGS="-allowProvisioningUpdates"
mkdir -p build-ios
pushd build-ios
$QMAKE \
    -spec macx-ios-clang \
    -config release \
    CONFIG+=iphoneos \
    CONFIG+=device \
    CONFIG+=ccache \
    ..
make qmake_all
make -j4
cd app

# Instruction based on
# https://medium.com/xcblog/xcodebuild-deploy-ios-app-from-command-line-c6defff0d8b8
xcodebuild \
    -project OpenTodoList.xcodeproj \
    -scheme OpenTodoList \
    -sdk iphoneos \
    -configuration AppStoreDistribution \
    archive \
    -archivePath $PWD/build/OpenTodoList.xcarchive \
    -allowProvisioningUpdates \
    -destination 'generic/platform=iOS'
xcodebuild \
    -exportArchive \
    -archivePath $PWD/build/OpenTodoList.xcarchive \
    -exportPath $PWD/build \
    -exportOptionsPlist $PWD/../../app/ios/exportOptions.plist \
    -allowProvisioningUpdates \
    -destination 'generic/platform=iOS'
popd

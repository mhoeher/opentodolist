#!/bin/bash

set -e

if [ -z "$IOS_PLATFORM" ]; then
  # In CI, only build for target, not simularot
  IOS_PLATFORM=OS64
fi

if [ -z "$IOS_TEAM_ID" ]; then
    # Default Team ID to use:
    IOS_TEAM_ID="786Z636JV9"
fi

mkdir -p build-ios-cmake
cd build-ios-cmake

export XCODEBUILD_FLAGS="-allowProvisioningUpdates"

cmake \
    -GXcode \
    -DCMAKE_TOOLCHAIN_FILE=../3rdparty/IosCMakeToolchain/ios.toolchain.cmake \
    -DPLATFORM="$IOS_PLATFORM" \
    -DDEPLOYMENT_TARGET=12.0 \
    -DCMAKE_PREFIX_PATH=$QT_DIR_IOS \
    -DENABLE_BITCODE=FALSE \
    -DCMAKE_CXX_COMPILER_LAUNCHER=ccache \
    -DCMAKE_C_COMPILER_LAUNCHER=ccache \
    -DQT_IOS_TEAM_ID="$IOS_TEAM_ID" \
    -DQT_IOS_EXPORT_ARCHIVE_XCODEBUILD_FLAGS="$XCODEBUILD_FLAGS" \
    ..

cmake --build . --config Release -- "$XCODEBUILD_FLAGS"

exit 0

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
    -allowProvisioningUpdates
xcodebuild \
    -exportArchive \
    -archivePath $PWD/build/OpenTodoList.xcarchive \
    -exportPath $PWD/build \
    -exportOptionsPlist $PWD/../../app/ios/exportOptions.plist \
    -allowProvisioningUpdates
popd

xcodebuild 
    -exportArchive 
    -archivePath /Users/martin/Projects/rpdev/opentodolist/build-ios-cmake/app/OpenTodoList.xcarchive 
    -exportOptionsPlist /Users/martin/Projects/rpdev/opentodolist/build-ios-cmake/app/OpenTodoListExportOptions.plist 
    -exportPath /Users/martin/Projects/rpdev/opentodolist/build-ios-cmake/app/OpenTodoListIpa 
    -allowProvisioningUpdates

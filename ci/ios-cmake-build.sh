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

if [ -n "$CONFIGURE_ONLY" ]; then
    exit 0
fi

cmake --build . --config Release -- "$XCODEBUILD_FLAGS"

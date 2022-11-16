#!/bin/bash

set -e

if [ ! -d "$QT_INSTALLATION_DIR" ]; then
    if [ -d "$HOME/Qt" ]; then
        QT_INSTALLATION_DIR="$HOME/Qt"
    else
        echo "The variable QT_INSTALLATION_DIR is not set"
        exit 1
    fi
fi
echo "Using Qt installation in $QT_INSTALLATION_DIR"

if [ -z "$QT_VERSION" ]; then
    QT_VERSION=$(ls "$QT_INSTALLATION_DIR" | grep -E '\d+\.\d+\.\d+' | sort -V | tail -n1)
fi
echo "Using Qt $QT_VERSION"

QT_DIR_IOS=$QT_INSTALLATION_DIR/$QT_VERSION/ios
QT_DIR=$QT_INSTALLATION_DIR/$QT_VERSION/macos

if [ -z "$IOS_TEAM_ID" ]; then
    # Default Team ID to use:
    IOS_TEAM_ID="786Z636JV9"
fi

mkdir -p build-ios-cmake
cd build-ios-cmake

export XCODEBUILD_FLAGS="-allowProvisioningUpdates"
export XCODE_ARCHIVE_FLAGS="-destination;generic/platform=iOS"

# /Users/mhoeher/Qt/Tools/CMake/CMake.app/Contents/bin/cmake -S /Users/mhoeher/Projects/rpdev/opentodolist -B /Users/mhoeher/Projects/rpdev/build-opentodolist-Qt_6_4_0_for_iOS -DCMAKE_GENERATOR:STRING=Xcode '-DCMAKE_PROJECT_INCLUDE_BEFORE:FILEPATH=/Users/mhoeher/Qt/Qt Creator.app/Contents/Resources/package-manager/auto-setup.cmake' -DQT_QMAKE_EXECUTABLE:FILEPATH=/Users/mhoeher/Qt/6.4.0/ios/bin/qmake -DCMAKE_PREFIX_PATH:PATH=/Users/mhoeher/Qt/6.4.0/ios -DCMAKE_C_COMPILER:FILEPATH=/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang -DCMAKE_CXX_COMPILER:FILEPATH=/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang++ -DCMAKE_TOOLCHAIN_FILE:FILEPATH=/Users/mhoeher/Qt/6.4.0/ios/lib/cmake/Qt6/qt.toolchain.cmake -DCMAKE_OSX_ARCHITECTURES:STRING=arm64 -DCMAKE_OSX_SYSROOT:STRING=iphoneos -DCMAKE_CXX_FLAGS_INIT:STRING=-DQT_QML_DEBUG -DCMAKE_XCODE_ATTRIBUTE_DEVELOPMENT_TEAM:STRING=786Z636JV9

cmake \
    -S .. \
    -B . \
    -DCMAKE_GENERATOR:STRING=Xcode \
    -DQT_QMAKE_EXECUTABLE:FILEPATH=$QT_DIR_IOS/bin/qmake \
    -DCMAKE_PREFIX_PATH:PATH=$QT_DIR_IOS \
    -DCMAKE_TOOLCHAIN_FILE:FILEPATH=$QT_DIR_IOS/lib/cmake/Qt6/qt.toolchain.cmake \
    -DCMAKE_OSX_ARCHITECTURES:STRING=arm64 \
    -DCMAKE_OSX_SYSROOT:STRING=iphoneos \
    -DCMAKE_XCODE_ATTRIBUTE_DEVELOPMENT_TEAM:STRING=$IOS_TEAM_ID \
    -DCMAKE_CXX_COMPILER_LAUNCHER=ccache \
    -DCMAKE_C_COMPILER_LAUNCHER=ccache \
    -DQT_HOST_PATH=$QT_DIR

#cmake \
#    -GXcode \
#    -DCMAKE_TOOLCHAIN_FILE=../3rdparty/IosCMakeToolchain/ios.toolchain.cmake \
#    -DDEPLOYMENT_TARGET=12.0 \
#    -DCMAKE_PREFIX_PATH=$QT_DIR_IOS \
#    -DENABLE_BITCODE=FALSE \
#    -DCMAKE_CXX_COMPILER_LAUNCHER=ccache \
#    -DCMAKE_C_COMPILER_LAUNCHER=ccache \
#    -DQT_IOS_TEAM_ID="$IOS_TEAM_ID" \
#    -DQT_IOS_EXPORT_ARCHIVE_XCODEBUILD_FLAGS="${XCODEBUILD_FLAGS}" \
#    -DQT_IOS_ARCHIVE_XCODEBUILD_FLAGS="${XCODE_ARCHIVE_FLAGS}" \
#    ..

if [ -n "$CONFIGURE_ONLY" ]; then
    exit 0
fi

cmake --build . --config Release -- "$XCODEBUILD_FLAGS"
xcodebuild -project OpenTodoList.xcodeproj -scheme OpenTodoList -archivePath OpenTodoList.xcarchive -destination generic/platform=iOS archive
xcodebuild -exportArchive -archivePath OpenTodoList.xcarchive -exportOptionsPlist ../app/ExportOptions.plist -exportPath OpenTodoList.ipa -allowProvisioningUpdates

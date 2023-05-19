#!/bin/bash

set -e

# Fetch secure files - see https://docs.gitlab.com/ee/ci/secure_files/index.html#use-secure-files-in-cicd-jobs
if [ -n "$CI" ]; then
    export SECURE_FILES_DOWNLOAD_PATH=.secure-files
    export QT_ANDROID_KEYSTORE_PATH=$PWD/$SECURE_FILES_DOWNLOAD_PATH/android_release.keystore
    curl --silent "https://gitlab.com/gitlab-org/incubation-engineering/mobile-devops/download-secure-files/-/raw/main/installer" | bash
    CMAKE_SIGNING_ARGS="-DQT_ANDROID_SIGN_AAB=ON"
else
    CMAKE_SIGNING_ARGS="-DQT_ANDROID_SIGN_AAB=OFF"
fi

if [ -z "$ANDROID_ABIS" ]; then
    ANDROID_ABIS=arm64-v8a
fi

case "$ANDROID_ABIS" in
    armeabi-v7a)
        VERSION_OFFSET=1
        QT_SUBDIR=android_armv7;
        ;;
    arm64-v8a)
        VERSION_OFFSET=2
        QT_SUBDIR=android_arm64_v8a;
        ;;
    x86_64)
        VERSION_OFFSET=4;
        QT_SUBDIR=android_x86_64;
        ;;
    x86)
        VERSION_OFFSET=3;
        QT_SUBDIR=android_x86;
        ;;
    *)
        echo "Unhandled Android architecture: $ANDROID_ABIS"
        exit 1
        ;;
esac

if [ ! -d "$QT_INSTALL_ROOT" ]; then
    if [ -d "$HOME/Qt" ]; then
        QT_INSTALL_ROOT="$HOME/Qt"
    else
        echo "The variable QT_INSTALL_ROOT is not set"
        exit 1
    fi
fi
echo "Using Qt installation in $QT_INSTALL_ROOT"

if [ -z "$QT_VERSION" ]; then
    QT_VERSION=$(ls "$QT_INSTALL_ROOT" | grep -E '\d+\.\d+\.\d+' | sort -V | tail -n1)
fi
echo "Using Qt $QT_VERSION"

if [ -z "$ANDROID_NDK_ROOT" ]; then
    echo "ANDROID_NDK_ROOT is unset!"
    exit 1
fi

if [ -z "$ANDROID_SDK_ROOT" ]; then
    echo "ANDROID_SDK_ROOT is unset!"
    exit 1
fi

export PATH=$QT_INSTALL_ROOT/$QT_VERSION/$QT_SUBDIR/bin:$PATH
export PROJECT_ROOT=$(cd $(dirname $0) && cd .. && pwd)
export ANDROID_NDK_HOME=$ANDROID_NDK_ROOT


cd $PROJECT_ROOT

PIPELINE_OFFSET=700
VERSIONCODE_OFFSET=650
if [ -z "$CI_PIPELINE_IID" ]; then
    CI_PIPELINE_IID=$PIPELINE_OFFSET
fi

let 'OPENTODOLIST_VERSION_CODE=VERSIONCODE_OFFSET + (CI_PIPELINE_IID - PIPELINE_OFFSET) * 5 + VERSION_OFFSET'

echo "Using Version Code $OPENTODOLIST_VERSION_CODE"

mkdir -p build-android
cd build-android
CMAKE_ABI_ARGS="-DANDROID_ABI=$ANDROID_ABIS"

qt-cmake \
    -GNinja \
    -DCMAKE_BUILD_TYPE=Release \
    -DANDROID_NATIVE_API_LEVEL=21 \
    -DANDROID_NDK=$ANDROID_NDK_ROOT \
    -DANDROID_STL=c++_shared \
    -DANDROID_SDK=$ANDROID_SDK_ROOT \
    $CMAKE_ABI_ARGS \
    $CMAKE_SIGNING_ARGS \
    -DOPENTODOLIST_ANDROID_VERSION_CODE=$OPENTODOLIST_VERSION_CODE \
    --fresh \
    ..
cmake --build .

# Build APK:
cmake --build . --target aab
OTL_VERSION="$(git describe --tags)"
cp app/android-build/OpenTodoList.apk \
    OpenTodoList-${ANDROID_ABIS}-${OTL_VERSION}.apk
cp app/android-build/build/outputs/bundle/release/android-build-release.aab \
    OpenTodoList-${ANDROID_ABIS}-${OTL_VERSION}.aab

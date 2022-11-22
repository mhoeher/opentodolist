#!/bin/bash

set -e

if [ -z "$ANDROID_ABIS" ]; then
    ANDROID_ABIS=arm64-v8a
fi

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

export PATH=$QT_INSTALL_ROOT/$QT_VERSION/android_$(echo $ANDROID_ABIS | sed -e s/-/_/g )/bin:$PATH
export PROJECT_ROOT=$(cd $(dirname $0) && cd .. && pwd)
export ANDROID_NDK_HOME=$ANDROID_NDK_ROOT


cd $PROJECT_ROOT

if [ -n "$CI" ]; then
    which ccache || (apt-get update -y && apt-get install -y ccache)
fi

# Add path to ccache symlinks to PATH:
export PATH=/usr/lib/ccache/:$PATH

# Ensure that `clang` is symlinked to `ccache`:
if [ ! -f /usr/lib/ccache/clang ]; then
    ln -s ../../bin/ccache /usr/lib/ccache/clang || true
fi

# Ensure that `clang++` is symlinked to `ccache`:
if [ ! -f /usr/lib/ccache/clang++ ]; then
    ln -s ../../bin/ccache /usr/lib/ccache/clang++ || true
fi

# Build APKs for each supported platform
case "$ANDROID_ABIS" in
    armeabi-v7a)
        VERSION_OFFSET=1
        break
        ;;
    arm64-v8a)
        VERSION_OFFSET=2
        break
        ;;
    x86_64)
        VERSION_OFFSET=4;
        break
        ;;
    x86)
        VERSION_OFFSET=3;
        break
        ;;
    *)
        echo "Unhandled Android architecture: $ANDROID_ABIS"
        exit 1
        ;;
esac

PIPELINE_OFFSET=700
VERSIONCODE_OFFSET=650
if [ -z "$CI_PIPELINE_IID" ]; then
    CI_PIPELINE_IID=$PIPELINE_OFFSET
fi

let 'OPENTODOLIST_VERSION_CODE=VERSIONCODE_OFFSET + (CI_PIPELINE_IID - PIPELINE_OFFSET) * 5 + VERSION_OFFSET'

echo "Using Version Code $OPENTODOLIST_VERSION_CODE"

echo $PATH
mkdir -p build-android
cd build-android
CMAKE_ABI_ARGS="-DANDROID_ABI=$ANDROID_ABIS"

qt-cmake \
    -GNinja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_PREFIX_PATH=$QT_ROOT \
    -DANDROID_NATIVE_API_LEVEL=21 \
    -DANDROID_NDK=$ANDROID_NDK_ROOT \
    -DANDROID_STL=c++_shared \
    -DCMAKE_FIND_ROOT_PATH=$QT_ROOT/$ANDROID_ABIS \
    -DANDROID_SDK=$ANDROID_SDK_ROOT \
    $CMAKE_ABI_ARGS \
    -DCMAKE_CXX_COMPILER_LAUNCHER=ccache \
    -DCMAKE_C_COMPILER_LAUNCHER=ccache \
    -DOPENTODOLIST_ANDROID_VERSION_CODE=$OPENTODOLIST_VERSION_CODE \
    ..
cmake --build .

# Build APK:
cmake --build . --target apk
cmake --build . --target aab
cp app/android-build/OpenTodoList.apk \
    OpenTodoList-${ANDROID_ABIS}-${OTL_VERSION}.apk
cp app/android-build/build/outputs/bundle/release/android-build-release.aab \
    OpenTodoList-${ANDROID_ABIS}-${OTL_VERSION}.aab

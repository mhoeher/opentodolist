#!/bin/bash

set -e

export PATH=$QT_ROOT/bin:$PATH
export PROJECT_ROOT=$(cd $(dirname $0) && cd .. && pwd)
export ANDROID_NDK_HOME=$ANDROID_NDK_ROOT
ANDROID_TARGET_SDK_VERSION=29

if [ -z "$ANDROID_ABIS" ]; then
    ANDROID_ABIS="armeabi-v7a arm64-v8a x86 x86_64"
fi

cd $PROJECT_ROOT

if [ -n "$CI" ]; then
    which ccache || (apt-get update -y && apt-get install -y ccache)
fi

# Add path to ccache symlinks to PATH:
export PATH=/usr/lib/ccache/:$PATH

# Ensure that `clang` is symlinked to `ccache`:
if [ ! -f /usr/lib/ccache/clang ]; then
    ln -s ../../bin/ccache /usr/lib/ccache/clang
fi

# Ensure that `clang++` is symlinked to `ccache`:
if [ ! -f /usr/lib/ccache/clang++ ]; then
    ln -s ../../bin/ccache /usr/lib/ccache/clang++
fi


mkdir -p build-android
cd build-android
unset CMAKE_ABI_ARGS
if [ -n "$BUILD_APK" ]; then
    CMAKE_ABI_ARGS="$CMAKE_ABI_ARGS -DANDROID_ABI=$ANDROID_ABIS"
fi
for abi in $ANDROID_ABIS; do
    CMAKE_ABI_ARGS="$CMAKE_ABI_ARGS -DANDROID_BUILD_ABI_$abi=ON"
done
cmake \
    -GNinja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_PREFIX_PATH=$QT_ROOT \
    -DANDROID_NATIVE_API_LEVEL=21 \
    -DANDROID_NDK=$ANDROID_NDK_ROOT \
    -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK_ROOT/build/cmake/android.toolchain.cmake \
    -DANDROID_STL=c++_shared \
    -DCMAKE_FIND_ROOT_PATH=$QT_ROOT \
    -DANDROID_SDK=$ANDROID_SDK_ROOT \
    $CMAKE_ABI_ARGS \
    -DCMAKE_CXX_COMPILER_LAUNCHER=ccache \
    -DCMAKE_C_COMPILER_LAUNCHER=ccache \
    -DANDROID_TARGET_SDK_VERSION=$ANDROID_TARGET_SDK_VERSION \
    ..
cmake --build .

# Fix up target SDK version in deployment config:
python \
    ../bin/set-android-deployment-target-sdk.py \
    android_deployment_settings.json $ANDROID_TARGET_SDK_VERSION \
    --android-package-source-directory $PWD/android-package-source

# Copy over package source:
cp -r ../app/android ./android-package-source

OTL_VERSION="$(git describe --tags)"

python ../bin/set-android-version-name \
    android-package-source/AndroidManifest.xml "$OTL_VERSION"

if [ -n "$BUILD_AAB" ]; then
    # Building Android AAB:
    python ../bin/increase-android-version-code \
        android-package-source/AndroidManifest.xml 0

    androiddeployqt \
        --output $PWD/android-build \
        --aab \
        --gradle \
        --release \
        --deployment bundled \
        --input android_deployment_settings.json
    cp android-build/build/outputs/bundle/release/android-build-release.aab \
        OpenTodoList-${OTL_VERSION}.aab
fi

if [ -n "$BUILD_APK" ]; then
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
    python ../bin/increase-android-version-code \
        android-package-source/AndroidManifest.xml $VERSION_OFFSET
    androiddeployqt \
        --output $PWD/android-build \
        --gradle \
        --release \
        --deployment bundled \
        --input android_deployment_settings.json
    cp android-build/build/outputs/apk/release/android-build-release-unsigned.apk \
        OpenTodoList-${ANDROID_ABIS}-${OTL_VERSION}.apk
fi

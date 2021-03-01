#!/bin/bash

set -e

export PATH=$QT_ROOT/bin:$PATH
export PROJECT_ROOT=$(cd $(dirname $0) && cd .. && pwd)
export ANDROID_NDK_HOME=$ANDROID_NDK_ROOT

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
    -DANDROID_TARGET_SDK_VERSION=29 \
    ..
cmake --build .

# Prepare builds for architecture specific APK builds:
python \
    ../bin/prepare-apk-deployment-settings.py \
    android_deployment_settings.json
for abi in $ANDROID_ABIS; do
    mkdir -p android-build-$abi/libs
    cp -r android-build/libs/$abi android-build-$abi/libs
done

OTL_VERSION="$(git describe --tags)"

# Building Android AAB:
python ../bin/increase-android-version-code \
    ../app/android/AndroidManifest.xml 0
python ../bin/set-android-version-name \
    ../app/android/AndroidManifest.xml "$OTL_VERSION"
androiddeployqt \
    --output $PWD/android-build \
    --aab \
    --gradle \
    --release \
    --deployment bundled \
    --input android_deployment_settings.json
cp android-build/build/outputs/bundle/release/android-build-release.aab \
    OpenTodoList-${OTL_VERSION}.aab

# Build APKs for each supported platform
for abi in $ANDROID_ABIS; do
    case "$abi" in
        armeabi-v7a)
            VERSION_OFFSET=1
            ;;
        arm64-v8a)
            VERSION_OFFSET=2
            ;;
        x86_64)
            VERSION_OFFSET=4;
            ;;
        x86)
            VERSION_OFFSET=3;
            ;;
        *)
            echo "Unhandled Android architecture: $abi"
            exit 1
            ;;
    esac
    python ../bin/increase-android-version-code \
        ../app/android/AndroidManifest.xml $VERSION_OFFSET
    androiddeployqt \
        --output $PWD/android-build-$abi \
        --gradle \
        --release \
        --deployment bundled \
        --input android_deployment_settings_${abi}.json
    cp android-build-$abi/build/outputs/apk/release/android-build-$abi-release-unsigned.apk \
        OpenTodoList-${abi}-${OTL_VERSION}.apk
done

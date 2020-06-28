#!/bin/bash

set -e

export PATH=$QT_ROOT/bin:$PATH
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
    ln -s ../../bin/ccache /usr/lib/ccache/clang
fi

# Ensure that `clang++` is symlinked to `ccache`:
if [ ! -f /usr/lib/ccache/clang++ ]; then
    ln -s ../../bin/ccache /usr/lib/ccache/clang++
fi


mkdir -p build-android
cd build-android
$QT_ROOT/bin/qmake \
    -spec android-clang \
    CONFIG+=release \
    CONFIG+=qlmdb_with_builtin_lmdb \
    CONFIG+=ccache \
    ANDROID_ABIS="$ANDROID_ABIS" \
    ..
make -j4

OTL_VERSION="$(git describe --tags)"

if [ `echo "$ANDROID_ABIS" | wc -w` -gt 1 ]; then
    # Building Android AAB:
    python ../bin/increase-android-version-code \
        ../app/android/AndroidManifest.xml 0
    python ../bin/set-android-version-name \
        ../app/android/AndroidManifest.xml "$OTL_VERSION"
    make install INSTALL_ROOT=$PWD/android
    rm -f $PWD/android/libs/*/libtst*.so # Remove unit tests
    androiddeployqt \
        --output $PWD/android \
        --verbose \
        --aab \
        --gradle \
        --release \
        --deployment bundled \
        --input app/android-OpenTodoList-deployment-settings.json
    cp android/build/outputs/bundle/release/android-release.aab \
        OpenTodoList.aab
else
    # Building a single APK
    case "$ANDROID_ABIS" in
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
            echo "Unhandled Android architecture: $ANDROID_ABIS"
            exit 1
            ;;
    esac
    python ../bin/increase-android-version-code \
        ../app/android/AndroidManifest.xml $VERSION_OFFSET
    python ../bin/set-android-version-name \
        ../app/android/AndroidManifest.xml "$OTL_VERSION"
    make install INSTALL_ROOT=$PWD/android
    rm -f $PWD/android/libs/*/libtst*.so # Remove unit tests
    androiddeployqt \
        --output $PWD/android \
        --verbose \
        --gradle \
        --release \
        --deployment bundled \
        --input app/android-OpenTodoList-deployment-settings.json
    cp android/build/outputs/apk/release/android-release-unsigned.apk \
        OpenTodoList-Android-${ANDROID_ABIS}.apk
fi

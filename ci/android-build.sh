#!/bin/bash

set -e

export PATH=$QT_ROOT/bin:$PATH
export PROJECT_ROOT=$(cd $(dirname $0) && cd .. && pwd)
export ANDROID_NDK_HOME=$ANDROID_NDK_ROOT

cd $PROJECT_ROOT

mkdir -p build-android
cd build-android
$QT_ROOT/bin/qmake \
    -spec android-clang \
    CONFIG+=release \
    CONFIG+=qlmdb_with_builtin_lmdb \
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
    pushd app
    make aab
    popd
    cp ./app/android-build/build/outputs/bundle/release/android-build-release.aab \
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
    pushd app
    make apk
    popd
    cp app/android-build/build/outputs/apk/debug/android-build-debug.apk \
       OpenTodoList-Android-$ANDROID_ABIS.apk
fi

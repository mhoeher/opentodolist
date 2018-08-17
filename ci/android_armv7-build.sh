#!/bin/bash

set -e

ninja-build --version || dnf install -y --nogpgcheck ninja-build

# Build prerequisites:
./ci/android/build-openssl \
    Setenv-android-armv7.sh \
    $PWD/pre-build/android/openssl/arm-linux-androideabi-4.9-api-18/

export PATH=$QT_ARM_ROOT/bin:$PATH

# Gather extra libraries to include in APK:
for lib in $(find $PWD/pre-build/android/ -name \*.so); do
    if [ -z "$EXTRA_LIBS" ]; then
        EXTRA_LIBS="$lib"
    else
        EXTRA_LIBS="$EXTRA_LIBS,$lib"
    fi
done

mkdir -p build-android-armv7
cd build-android-armv7
cmake \
    -GNinja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_COMPILER:STRING=$ANDROID_NDK_ROOT/toolchains/arm-linux-androideabi-4.9/prebuilt/linux-x86_64/bin/arm-linux-androideabi-g++ \
    -DCMAKE_C_COMPILER:STRING=$ANDROID_NDK_ROOT/toolchains/arm-linux-androideabi-4.9/prebuilt/linux-x86_64/bin/arm-linux-androideabi-gcc \
    -DCMAKE_PREFIX_PATH:STRING=$QT_ARM_ROOT \
    -DQT_QMAKE_EXECUTABLE:STRING=$QT_ARM_ROOT/bin/qmake \
    -DCMAKE_SYSTEM_NAME=Android \
    -DCMAKE_SYSTEM_VERSION=16 \
    -DCMAKE_ANDROID_ARCH_ABI=armeabi-v7a \
    -DCMAKE_ANDROID_STL_TYPE=gnustl_shared \
    -DANDROID_SDK_ROOT=$ANDROID_SDK_ROOT \
    -DOPENTODOLIST_ANDROID_EXTRA_LIBS="$EXTRA_LIBS" \
    ..
cmake --build .

# Prepare the Android Manifest:
#   - Set version name from git
#   - Increase version code by one
OTL_VERSION="$(git describe --tags)"
../bin/increase-android-version-code ../app/android/AndroidManifest.xml
../bin/set-android-version-name ../app/android/AndroidManifest.xml \
    "$OTL_VERSION"

# Build the APK:
cmake --build . -- OpenTodoList-apk

cp ./app/OpenTodoList-apk-build/build/outputs/apk/OpenTodoList-apk-build-release-unsigned.apk \
    OpenTodoList-Android-armv7.apk

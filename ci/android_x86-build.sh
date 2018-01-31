#!/bin/bash

set -e

# Build prerequisites:
./ci/android/build-openssl \
    Setenv-android-x86.sh \
    $PWD/pre-build/android/openssl/x86-4.9-api-18/

export PATH=$QT_X86_ROOT/bin:$PATH

mkdir -p build-android-x86
cd build-android-x86
qmake CONFIG+=release ..
make -j4
make install INSTALL_ROOT=$PWD/android

# Prepare the Android Manifest:
#   - Set version name from git
#   - Increase version code by two
OTL_VERSION="$(git describe --tags)"
../bin/increase-android-version-code ../app/android/AndroidManifest.xml
../bin/increase-android-version-code ../app/android/AndroidManifest.xml
../bin/set-android-version-name ../app/android/AndroidManifest.xml \
    "$OTL_VERSION"

androiddeployqt \
    --verbose \
    --output android \
    --input app/android-libOpenTodoList.so-deployment-settings.json \
    --deployment bundled \
    --android-platform android-26 \
    --jdk $JAVA_HOME \
    --gradle \
    --release

cp android/build/outputs/apk/android-release-unsigned.apk \
    OpenTodoList-Android-x86.apk

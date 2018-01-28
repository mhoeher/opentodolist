#!/bin/bash

set -e

export PATH=$QT_ARM_ROOT/bin:$PATH

mkdir -p build-android-armv7
cd build-android-armv7
qmake CONFIG+=release ..
make -j4
make install INSTALL_ROOT=$PWD/android

# Prepare the Android Manifest:
#   - Set version name from git
#   - Increase version code by one
OTL_VERSION="$(git describe --tags)"
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
    OpenTodoList-Android-armv7.apk

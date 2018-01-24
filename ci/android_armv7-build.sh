#!/bin/bash

set -e

export ANDROID_NDK_ROOT=/opt/android/android-ndk-r16b/
export ANDROID_SDK_ROOT=/opt/android
export PATH=/opt/Qt/5.10.0/android_armv7/bin:$PATH

mkdir -p build-android-armv7
cd build-android-armv7
qmake CONFIG+=release ..
make -j4
make install INSTALL_ROOT=$PWD/android
androiddeployqt \
    --verbose \
    --output android \
    --input app/android-libOpenTodoList.so-deployment-settings.json \
    --deployment bundled \
    --android-platform android-26 \
    --jdk /usr/lib/jvm/java-1.8.0 \
    --gradle

#!/bin/bash

set -e

cd build-android-armv7
jarsigner \
    -sigalg SHA1withRSA \
    -digestalg SHA1 \
    -keystore "$OPENTODOLIST_KEYSTORE" \
    -storepass "$OPENTODOLIST_KEYSTORE_SECRET" \
    OpenTodoList-Android-armv7.apk "$OPENTODOLIST_KEYSTORE_ALIAS"
$ANDROID_SDK_ROOT/build-tools/*/zipalign \
    -v 4 \
    OpenTodoList-Android-armv7.apk \
    OpenTodoList-Android-armv7-aligned.apk
cd ..

cd build-android-arm64_v8a/
jarsigner \
    -sigalg SHA1withRSA \
    -digestalg SHA1 \
    -keystore "$OPENTODOLIST_KEYSTORE" \
    -storepass "$OPENTODOLIST_KEYSTORE_SECRET" \
    OpenTodoList-Android-arm64_v8a.apk "$OPENTODOLIST_KEYSTORE_ALIAS"
$ANDROID_SDK_ROOT/build-tools/*/zipalign \
    -v 4 \
    OpenTodoList-Android-arm64_v8a.apk \
    OpenTodoList-Android-arm64_v8a-aligned.apk
cd ..

cd build-android-x86/
jarsigner \
    -sigalg SHA1withRSA \
    -digestalg SHA1 \
    -keystore "$OPENTODOLIST_KEYSTORE" \
    -storepass "$OPENTODOLIST_KEYSTORE_SECRET" \
    OpenTodoList-Android-x86.apk "$OPENTODOLIST_KEYSTORE_ALIAS"
$ANDROID_SDK_ROOT/build-tools/*/zipalign \
    -v 4 \
    OpenTodoList-Android-x86.apk \
    OpenTodoList-Android-x86-aligned.apk
cd ..

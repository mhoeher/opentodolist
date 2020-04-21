#!/bin/bash

set -e

pushd build-android
for arch in armeabi-v7a arm64-v8a x86_64 x86; do
    jarsigner \
        -sigalg SHA1withRSA \
        -digestalg SHA1 \
        -keystore "$OPENTODOLIST_KEYSTORE" \
        -storepass "$OPENTODOLIST_KEYSTORE_SECRET" \
        OpenTodoList-Android-${arch}.apk "$OPENTODOLIST_KEYSTORE_ALIAS"
    $ANDROID_SDK_ROOT/build-tools/*/zipalign \
        -v 4 \
        OpenTodoList-Android-${arch}.apk \
        OpenTodoList-Android-${arch}-aligned.apk
done
jarsigner \
    -sigalg SHA1withRSA \
    -digestalg SHA1 \
    -keystore "$OPENTODOLIST_KEYSTORE" \
    -storepass "$OPENTODOLIST_KEYSTORE_SECRET" \
    OpenTodoList.aab "$OPENTODOLIST_KEYSTORE_ALIAS"
$ANDROID_SDK_ROOT/build-tools/*/zipalign \
    -v 4 \
    OpenTodoList-Android.aab \
    OpenTodoList-Android-aligned.aab
popd

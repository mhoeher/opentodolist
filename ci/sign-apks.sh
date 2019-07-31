#!/bin/bash

set -e

for arch in armv7 arm64_v8a x86 x86_64; do
    pushd build-android-$arch
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
    popd
done

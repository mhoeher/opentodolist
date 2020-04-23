#!/bin/bash

set -e

# Note: Starting from some recent (2020-04-21) Android SDK release,
# we have to run zipalign twice: The first round will fail, when we
# rerun on the intermediate file, it succeeds. See also
# this SO post: https://stackoverflow.com/a/38074885/6367098

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
    OpenTodoList.aab \
    OpenTodoList-aligned.aab
popd

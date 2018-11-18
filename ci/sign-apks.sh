#!/bin/bash

set -e

echo "Using keystore $OPENTODOLIST_KEYSTORE with alias $OPENTODOLIST_KEYSTORE_ALIAS"
ls -lZ $OPENTODOLIST_KEYSTORE

cd build-android-armv7
jarsigner \
    -sigalg SHA1withRSA \
    -digestalg SHA1 \
    -keystore "$OPENTODOLIST_KEYSTORE" \
    -storepass "$OPENTODOLIST_KEYSTORE_SECRET" \
    OpenTodoList-Android-armv7.apk "$OPENTODOLIST_KEYSTORE_ALIAS"
$ANDROID_SDK_ROOT/build-tools/27.0.3/zipalign \
    -v 4 \
    OpenTodoList-Android-armv7.apk \
    OpenTodoList-Android-armv7-aligned.apk
cd ..

cd build-android-x86/
jarsigner \
    -sigalg SHA1withRSA \
    -digestalg SHA1 \
    -keystore "$OPENTODOLIST_KEYSTORE" \
    -storepass "$OPENTODOLIST_KEYSTORE_SECRET" \
    OpenTodoList-Android-x86.apk "$OPENTODOLIST_KEYSTORE_ALIAS"
$ANDROID_SDK_ROOT/build-tools/27.0.3/zipalign \
    -v 4 \
    OpenTodoList-Android-x86.apk \
    OpenTodoList-Android-x86-aligned.apk
cd ..

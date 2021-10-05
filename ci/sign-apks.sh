#!/bin/bash

set -e

pushd build-android
for android_file in *.apk *.aab; do
    if [ -f "$android_file" ]; then
        filename_extension="${android_file##*.}"
        android_file_out=$(basename $android_file .$filename_extension)-aligned-.$filename_extension

        # Align the APK:
        $ANDROID_SDK_ROOT/build-tools/*/zipalign \
            -v 4 \
            $android_file \
            $android_file_out

        # Sign the aligned APK:
        APKSIGNER_ARGS="--ks $OPENTODOLIST_KEYSTORE --ks-pass env:OPENTODOLIST_KEYSTORE_SECRET --ks-key-alias $OPENTODOLIST_KEYSTORE_ALIAS"
        if [ -n "$ANDROID_MIN_SDK_INT" ]; then
            APKSIGNER_ARGS="$APKSIGNER_ARGS --min-sdk-version $ANDROID_MIN_SDK_INT"
        fi
        /opt/android-sdk/build-tools/*/apksigner sign \
            $APKSIGNER_ARGS $android_file_out
    fi
done
popd

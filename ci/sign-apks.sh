#!/bin/bash

set -e

# Note: Starting from some recent (2020-04-21) Android SDK release,
# we have to run zipalign twice: The first round will fail, when we
# rerun on the intermediate file, it succeeds. See also
# this SO post: https://stackoverflow.com/a/38074885/6367098

pushd build-android
for android_file in *.apk *.aab; do
    jarsigner \
        -sigalg SHA1withRSA \
        -digestalg SHA1 \
        -keystore "$OPENTODOLIST_KEYSTORE" \
        -storepass "$OPENTODOLIST_KEYSTORE_SECRET" \
        $android_file "$OPENTODOLIST_KEYSTORE_ALIAS"
    $ANDROID_SDK_ROOT/build-tools/*/zipalign \
        -v 4 \
        $android_file \
        $(basename $(basename $android_file .aab) .apk)-aligned.apk
done
popd

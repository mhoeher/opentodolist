#!/bin/bash

set -e

BUILD_DIR=$PWD/build-macos

rm -rf $BUILD_DIR

if [ ! -d "$QT_DIR" ]; then
    echo "The variable QT_DIR is not set"
    exit 1
fi

QTSDK=$QT_DIR

#rm -rf $BUILD_DIR
mkdir -p $BUILD_DIR
cd $BUILD_DIR

$QTSDK/bin/qmake \
    -config release \
    CONFIG+=with_update_service \
    CONFIG+=qlmdb_with_static_libs \
    CONFIG+=synqclient_with_static_libs \
    CONFIG+=ccache \
    ..
make -j4
make check

# Include Qt Runtime in App Bundle. Also sign the bundle
# and prepare it for notarization:
$QTSDK/bin/macdeployqt \
    app/OpenTodoList.app/ \
    -qmldir=../app \
    -appstore-compliant \
    -sign-for-notarization="Developer ID Application: Martin Hoeher (786Z636JV9)"

# Create a zip archive suitable for uploading to the notarization
# service:
ditto \
    -ck --rsrc \
    --sequesterRsrc \
    --keepParent \
    "app/OpenTodoList.app" "app/OpenTodoList.zip"

# Make sure the app has been signed:
codesign -v app/OpenTodoList.app

# Upload the archive for notarization:
REQUEST_UUID=$(xcrun altool \
    --notarize-app \
    -t osx \
    -f "app/OpenTodoList.zip" \
    --primary-bundle-id="net.rpdev.opentodolist.zip" \
    -u "martin@rpdev.net" \
    -p "$OPENTODOLIST_STORE_KEY" | \
    grep 'RequestUUID =' | awk '{ print $3; }')

i="0"
while [ $i -lt 120 ]; do
    echo "Waiting for notarization to complete..."
    sleep 30
    STATUS=$(xcrun altool \
        --notarization-info $REQUEST_UUID \
        -u "martin@rpdev.net" \
        -p "$OPENTODOLIST_STORE_KEY" | \
        grep 'Status:' | awk '{ print $2 $3; }')
    echo "Notarization status is: ${STATUS}"
    case $STATUS in
        success)
            echo "Notarization finished successfully..."
            break
        ;;
        "inprogress")
            echo "Notarization still ongoing..."
            continue
        ;;
        *)
            echo "Notarization finished with an error!"
            false
        ;;
    esac
    i=$[$i+1]
done

# Include the notarization ticket in the app bundle:
xcrun stapler staple "app/OpenTodoList.app"

# Upload the App Bundle:
xcrun altool \
    --upload-app \
    -f "app/OpenTodoList.app" \
    -u "martin@rpdev.net" \
    -p "$OPENTODOLIST_STORE_KEY" || true

# Prepare a "beautified" folder:
cd app
mkdir dmg.in
cp -R OpenTodoList.app dmg.in
cp ../../templates/macos/DS_Store ./dmg.in/.DS_Store
cd dmg.in
ln -s /Applications ./Applications
cd ../

# Create DMG file:
i="0"
while [ $i -lt 5 ]; do
    if ! hdiutil create \
        -volname OpenTodoList \
        -srcfolder ./dmg.in \
        -ov -format UDZO \
        OpenTodoList.dmg; then
        echo "Creating disk image failed - retrying in 30s"
        sleep 30
    else
        break
    fi
    i=$[$i+1]
done

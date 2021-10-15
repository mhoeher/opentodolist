#!/bin/bash

set -e

BUILD_DIR=$PWD/build-macos

rm -rf $BUILD_DIR

if [ ! -d "$QT_INSTALLATION_DIR" ]; then
    echo "The variable QT_INSTALLATION_DIR is not set"
    exit 1
fi

if [ -z "$QT_VERSION" ]; then
  echo "The variable QT_VERSION is not set"
  exit 1
fi

if [ -z "$OSX_DEPLOYMENT_TARGET" ]; then
    OSX_DEPLOYMENT_TARGET=10.13
fi

QT_DIR=$QT_INSTALLATION_DIR/$QT_VERSION/clang_64

#rm -rf $BUILD_DIR
mkdir -p $BUILD_DIR
cd $BUILD_DIR

cmake \
    -GNinja \
    -DCMAKE_OSX_DEPLOYMENT_TARGET=$OSX_DEPLOYMENT_TARGET \
    -DCMAKE_PREFIX_PATH=$QT_DIR \
    -DCMAKE_BUILD_TYPE=Release \
    -DOPENTODOLIST_WITH_UPDATE_SERVICE=ON \
    -DCMAKE_CXX_COMPILER_LAUNCHER=ccache \
    -DCMAKE_C_COMPILER_LAUNCHER=ccache \
    $CMAKE_EXTRA_FLAGS \
    ..
cmake --build .
cmake --build . --target test

# For each localized file, create an empty "lproj" folder in the
# app bundle - this causes macOS to localize the system menu
# (see https://bugs.kde.org/show_bug.cgi?id=432685#c3):
for ts_file in ../app/translations/*.ts; do
    lang="$(basename $ts_file | tr - .  | cut -f 2 -d .)"
    mkdir -p app/OpenTodoList.app/Contents/Resources/${lang}.lproj
    echo '"Empty Translation" = "Empty Translation";' > app/OpenTodoList.app/Contents/Resources/${lang}.lproj/Empty.strings
done

# Include Qt translations:
mkdir -p app/OpenTodoList.app/Contents/translations
cp $QT_DIR/translations/qt*.qm app/OpenTodoList.app/Contents/translations/


# Include Qt Runtime in App Bundle. Also sign the bundle
# and prepare it for notarization:
pushd app
$QT_DIR/bin/macdeployqt \
    OpenTodoList.app/ \
    -qmldir=../../app \
    -appstore-compliant \
    -sign-for-notarization="Developer ID Application: Martin Hoeher (786Z636JV9)"
popd

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

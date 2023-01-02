#!/bin/bash

set -e

BUILD_DIR=$PWD/build-macos

rm -rf $BUILD_DIR

if [ ! -d "$QT_INSTALLATION_DIR" ]; then
    if [ -d "$HOME/Qt" ]; then
        QT_INSTALLATION_DIR="$HOME/Qt"
    else
        echo "The variable QT_INSTALLATION_DIR is not set"
        exit 1
    fi
fi
echo "Using Qt installation in $QT_INSTALLATION_DIR"

if [ -z "$QT_VERSION" ]; then
    QT_VERSION=$(ls "$QT_INSTALLATION_DIR" | grep -E '\d+\.\d+\.\d+' | sort -V | tail -n1)
fi
echo "Using Qt $QT_VERSION"

if [ -z "$MACOS_TEAM_ID" ]; then
    # Default Team ID to use:
    MACOS_TEAM_ID="786Z636JV9"
fi

QT_DIR=$QT_INSTALLATION_DIR/$QT_VERSION/macos

#rm -rf $BUILD_DIR
mkdir -p $BUILD_DIR
cd $BUILD_DIR

$QT_DIR/bin/qt-cmake \
    -GNinja \
    -DCMAKE_PREFIX_PATH=$QT_DIR \
    -DCMAKE_BUILD_TYPE=Release \
    -DOPENTODOLIST_WITH_UPDATE_SERVICE=ON \
    -DCMAKE_CXX_COMPILER_LAUNCHER=ccache \
    -DCMAKE_C_COMPILER_LAUNCHER=ccache \
    -DCMAKE_OSX_ARCHITECTURES="x86_64" \
    $CMAKE_EXTRA_FLAGS \
    ..
cmake --build .
# TODO: Tests on macos currently fail sometimes - this probably is a race condition that we should urgently fix!
# For now, try up to 3 times to repeat.
cmake --build . --target test || cmake --build . --target test || cmake --build . --target test

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


###########################################
# Create app for distribution via website #
###########################################

# Include Qt Runtime in App Bundle. Also sign the bundle
# and prepare it for notarization:
mkdir -p dist-web
cp -r app/OpenTodoList.app dist-web
pushd dist-web
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
    "dist-web/OpenTodoList.app" "dist-web/OpenTodoList.zip"

# Make sure the app has been signed:
codesign -v dist-web/OpenTodoList.app

# Upload the archive for notarization (see
# https://developer.apple.com/documentation/security/notarizing_macos_software_before_distribution/customizing_the_notarization_workflow
# for details):
xcrun notarytool submit \
    "dist-web/OpenTodoList.zip" \
    --wait \
    --apple-id "martin@rpdev.net" \
    --team-id "$MACOS_TEAM_ID" \
    --password "$OPENTODOLIST_STORE_KEY"

# Include the notarization ticket in the app bundle:
xcrun stapler staple "dist-web/OpenTodoList.app"

# Prepare a "beautified" folder:
cd dist-web
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

cd ..
cp dist-web/OpenTodoList.dmg app/


#################################################
# Create Package for Distribution via App Store #
#################################################

mkdir dist-store
cp -r app/OpenTodoList.app dist-store

pushd dist-store
$QT_DIR/bin/macdeployqt \
    OpenTodoList.app/ \
    -qmldir=../../app \
    -appstore-compliant \
    -sign-for-notarization="3rd Party Mac Developer Application: Martin Hoeher (786Z636JV9)"
popd
xcrun codesign \
    -s "3rd Party Mac Developer Application: Martin Hoeher (786Z636JV9)" \
    -v -f \
    -o runtime \
    --entitlements ../app/OpenTodoList.entitlements \
    "dist-store/OpenTodoList.app"

# Upload the App Bundle:
xcrun productbuild --component "dist-store/OpenTodoList.app" /Applications "dist-store/OpenTodoList.pkg"
xcrun productsign --sign "3rd Party Mac Developer Installer: Martin Hoeher (786Z636JV9)" "dist-store/OpenTodoList.pkg" "dist-store/OpenTodoList-signed.pkg"
#xcrun productsign --sign "Developer ID Installer: Martin Hoeher (786Z636JV9)" "dist-store/OpenTodoList.pkg" "dist-store/OpenTodoList-signed.pkg"
xcrun altool --validate-app \
    -f "dist-store/OpenTodoList-signed.pkg" \
    -t macos \
    -u "martin@rpdev.net" \
    -p "$OPENTODOLIST_STORE_KEY"


#!/bin/bash

set -e

# Install secrets when running in CI:
. ci/apple/get-secrets.sh

# Install Qt in CI if needed
export QT_INSTALL_IOS="1"
bash ci/apple/macos-install-qt.sh

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

QT_DIR_IOS=$QT_INSTALLATION_DIR/$QT_VERSION/ios
QT_DIR=$QT_INSTALLATION_DIR/$QT_VERSION/macos

export PATH=$QT_INSTALLATION_DIR/Tools/Ninja:$QT_INSTALLATION_DIR/Tools/CMake/CMake.app/Contents/bin:$PATH


# The iOS build is currently a bit unstable... as setting up the environment
# is quite costly, we rather retry the actual build several times (cleaning up
# in between) rather than giving up immediately.
i=0
while [[ $i -lt 5 ]]
do
    ((i++))
    rm -rf build-ios-cmake
    mkdir -p build-ios-cmake
    cd build-ios-cmake

    $QT_DIR_IOS/bin/qt-cmake \
        -S .. \
        -B . \
        -DCMAKE_BUILD_TYPE=Release \
        -DQT_QMAKE_EXECUTABLE:FILEPATH=$QT_DIR_IOS/bin/qmake \
        -DCMAKE_PREFIX_PATH:PATH=$QT_DIR_IOS \
        -DCMAKE_OSX_ARCHITECTURES:STRING=arm64 \
        -DCMAKE_OSX_SYSROOT:STRING=iphoneos \
        -DQT_HOST_PATH=$QT_DIR

    if [ -n "$CONFIGURE_ONLY" ]; then
        exit 0
    fi

    # cmake --build . --config Release -- "$XCODEBUILD_FLAGS" ## Leads to "Archive Failed" errors in next step - but we need at least CMake 3.25.0
    if xcodebuild -project OpenTodoList.xcodeproj -scheme opentodolist-common -destination generic/platform=iOS -quiet -configuration Release && \
        xcodebuild -project OpenTodoList.xcodeproj -scheme OpenTodoList -archivePath OpenTodoList.xcarchive -destination generic/platform=iOS archive -quiet -configuration Release && \
        xcodebuild -exportArchive -archivePath OpenTodoList.xcarchive -exportOptionsPlist ../app/ExportOptions.plist -exportPath OpenTodoList.ipa -quiet -configuration Release; then
        exit 0
    else
        echo "Build attempt $i failed"
    fi
    cd ..
done

# Still here? Then we didn't get a succesful build!
exit 1

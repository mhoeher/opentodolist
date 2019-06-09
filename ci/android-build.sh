#!/bin/bash

set -e

export PATH=$QT_ROOT/bin:$PATH
export PROJECT_ROOT=$PWD

TARGET_ARCH="$(basename "$QT_ROOT" | sed -e 's/^android_//')"

case $TARGET_ARCH in
    arm64_v8a)
        OPENSSL_FLAGS="android-arm64 -D__ANDROID_API__=21"
        ANDROID_VERSION_CODE_OFFSET=2
    ;;
    armv7)
        OPENSSL_FLAGS="android-arm -D__ANDROID_API__=19"
        ANDROID_VERSION_CODE_OFFSET=0
    ;;
    # We currently do not build for x86_64, as there are no pre-compiled
    # Qt releases for this Android platform.
#    x86_64)
#        OPENSSL_FLAGS="android-x86_64 -D__ANDROID_API__=21"
#        ANDROID_VERSION_CODE_OFFSET=3
#    ;;
    x86)
        OPENSSL_FLAGS="android-x86 -D__ANDROID_API__=19"
        ANDROID_VERSION_CODE_OFFSET=1
    ;;
    *)
        echo "Unsupported Android Target Arch"
        /bin/false
    ;;
esac

CMAKE_FLAGS_3RDPARTY="\
    -GNinja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_PREFIX_PATH:STRING=$QT_ROOT \
    -DCMAKE_CXX_COMPILER:STRING=$ANDROID_NDK_ROOT/toolchains/llvm/prebuilt/linux-x86_64/bin/clang++ \
    -DCMAKE_C_COMPILER:STRING=$ANDROID_NDK_ROOT/toolchains/llvm/prebuilt/linux-x86_64/bin/clang \
    -DQT_QMAKE_EXECUTABLE:STRING=$QT_ROOT/bin/qmake \
    -DCMAKE_INSTALL_PREFIX=$QT_ROOT \
    -DCMAKE_TOOLCHAIN_FILE=$PROJECT_ROOT/3rdparty/qt-cmake-android/cmake/toolchain-android.cmake
"

CMAKE_FLAGS="\
    $CMAKE_FLAGS_3RDPARTY
"

if [ -n "$CI" ]; then
    # Install ECM:
    pushd 3rdparty/KDE/extra-cmake-modules
    rm -rf build-android-$TARGET_ARCH
    mkdir -p build-android-$TARGET_ARCH
    cd build-android-$TARGET_ARCH
    cmake $CMAKE_FLAGS_3RDPARTY ..
    cmake --build .
    cmake --build . --target install
    popd

    # Install KDE syntax highlighting
    pushd 3rdparty/KDE/syntax-highlighting/
    rm -rf build-android-$TARGET_ARCH
    mkdir -p build-android-$TARGET_ARCH
    cd build-android-$TARGET_ARCH
    cmake $CMAKE_FLAGS_3RDPARTY -DKF5_HOST_TOOLING=/usr ..
    cmake --build .
    cmake --build . --target install
    popd

    rm -rf build-android-openssl-$TARGET_ARCH
    mkdir -p build-android-openssl-$TARGET_ARCH
    pushd build-android-openssl-$TARGET_ARCH
    wget https://www.openssl.org/source/openssl-1.1.1c.tar.gz
    tar xf openssl-*.tar.gz
    rm openssl-*.tar.gz
    cd openssl-*
    export ANDROID_NDK_HOME="$ANDROID_NDK_ROOT"
    OLD_PATH="$PATH"
    export PATH="$ANDROID_NDK_ROOT/toolchains/llvm/prebuilt/linux-x86_64/bin/:$PATH"
    ./Configure $OPENSSL_FLAGS
    make \
        CALC_VERSIONS="SHLIB_COMPAT=; SHLIB_SOVER=" \
        build_libs -j4
    ANDROID_EXTRA_LIBS="$PWD/libcrypto.so;$PWD/libssl.so"
    export PATH="$OLD_PATH"
    popd
fi

mkdir -p build-android-$TARGET_ARCH
cd build-android-$TARGET_ARCH
cmake \
    $CMAKE_FLAGS \
    "-DANDROID_EXTRA_LIBS=$ANDROID_EXTRA_LIBS" \
    -DANDROID_PACKAGE_SOURCE_DIR=$PROJECT_ROOT/app/android \
    ..
cmake --build . -- opentodolist-translations
cmake --build .


# Prepare the Android Manifest:
#   - Set version name from git
#   - Calculate version code from pipeline IID
OTL_VERSION="$(git describe --tags)"
../bin/increase-android-version-code \
    ../app/android/AndroidManifest.xml \
    $ANDROID_VERSION_CODE_OFFSET
../bin/set-android-version-name \
    ../app/android/AndroidManifest.xml \
    "$OTL_VERSION"

# Build the APK:
cmake --build . -- OpenTodoList-apk

cp ./app/OpenTodoList-apk-build/build/outputs/apk/release/OpenTodoList-apk-build-release-unsigned.apk \
    OpenTodoList-Android-${TARGET_ARCH}.apk

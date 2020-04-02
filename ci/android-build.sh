#!/bin/bash

set -e

export PATH=$QT_ROOT/bin:$PATH
export PROJECT_ROOT=$PWD

TARGET_ARCH="$(basename "$QT_ROOT" | sed -e 's/^android_//')"

# Note: A nice instruction on how to build OpenSSL is taken from
# https://www.tal.org/tutorials/openssl_qt_android.

case $TARGET_ARCH in
    arm64_v8a)
        OPENSSL_ARCH=arm64
        OPENSSL_API=21
        #OPENSSL_CROSS_COMPILE=/bin/aarch64-linux-android-
        ANDROID_VERSION_CODE_OFFSET=2
    ;;
    armv7)
        OPENSSL_ARCH=arm
        OPENSSL_API=19
        #OPENSSL_CROSS_COMPILE=/bin/arm-linux-androideabi-
        ANDROID_VERSION_CODE_OFFSET=0
    ;;
    x86_64)
        OPENSSL_ARCH=x86_64
        OPENSSL_API=21
        #OPENSSL_CROSS_COMPILE=/bin/x86_64-linux-android-
        ANDROID_VERSION_CODE_OFFSET=3
    ;;
    x86)
        OPENSSL_ARCH=x86
        OPENSSL_API=19
        #OPENSSL_CROSS_COMPILE=/bin/i686-linux-android-
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

if [ -n "$CI" ]; then
    # Ensure Python is installed:
    which python || dnf install --refresh -y --nogpgcheck python

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
     wget https://www.openssl.org/source/old/1.1.1/openssl-1.1.1b.tar.gz
     tar xf openssl-*.tar.gz
     rm openssl-*.tar.gz
    cd openssl-*
     PATH=$ANDROID_NDK_ROOT/toolchains/llvm/prebuilt/linux-x86_64/bin/:$PATH \
         CC=clang \
         ANDROID_API=$OPENSSL_API bash -c \
         "\
         (./Configure shared android-$OPENSSL_ARCH -D__ANDROID_API__=$OPENSSL_API && \
          make -j4 SHLIB_VERSION_NUMBER= SHLIB_EXT=_1_1.so build_libs) \
          "
    ANDROID_EXTRA_LIBS="$PWD/libcrypto_1_1.so $PWD/libssl_1_1.so"
    popd
fi

rm -rf build-android-$TARGET_ARCH
mkdir -p build-android-$TARGET_ARCH
cd build-android-$TARGET_ARCH
qmake \
    CONFIG+=release \
    CONFIG+=qlmdb_with_builtin_lmdb \
    ANDROID_EXTRA_LIBS="$ANDROID_EXTRA_LIBS" \
    ..
make -j4


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
make install INSTALL_ROOT=$PWD/android
rm -f $PWD/android/libs/*/libtst*.so # Remove unit tests
androiddeployqt \
    --output $PWD/android \
    --verbose \
    --gradle \
    --release \
    --deployment bundled \
    --input app/android-libOpenTodoList.so-deployment-settings.json

cp android/build/outputs/apk/release/android-release-unsigned.apk \
    OpenTodoList-Android-${TARGET_ARCH}.apk

# Note: A nice instruction on how to build OpenSSL is taken from
# https://www.tal.org/tutorials/openssl_qt_android.

android {
URL=https://www.openssl.org/source/old/1.1.1/openssl-1.1.1b.tar.gz
ARCHIVE=openssl-1.1.1b.tar.gz
SOURCE_DIR=openssl-1.1.1b
SHA256SUM="5c557b023230413dfb0756f3137a13e6d726838ccd1430888ad15bfb2b43ea4b  openssl-1.1.1b.tar.gz"

# Fetch OpenSSL library:
fetch_openssl.target = $$ARCHIVE
fetch_openssl.commands = curl -o "$$ARCHIVE" "$$URL"
QMAKE_EXTRA_TARGETS *= fetch_openssl


# Verify OpenSSL library
verify_openssl.target = verify_openssl
verify_openssl.commands = \
    echo "$$SHA256SUM" > sha256sum.txt && \
    sha256sum -c sha256sum.txt && \
    touch .verify_openssl
verify_openssl.depends = fetch_openssl
QMAKE_EXTRA_TARGETS *= verify_openssl


# Extract library once:
extract_openssl.target = $$SOURCE_DIR
extract_openssl.commands = \
    tar -xf $$ARCHIVE
extract_openssl.depends = verify_openssl
QMAKE_EXTRA_TARGETS *= extract_openssl


# Build OpenSSL for 32bit ARM:
contains(ANDROID_ABIS, armeabi-v7a) {
    openssl_arm.target = .openssl-arm
    openssl_arm.commands = \
        cp -r $$SOURCE_DIR openssl-arm && \
        cd openssl-arm && \
        export CC=clang && \
        export ANDROID_API=19 && \
        export PATH="$(ANDROID_NDK_ROOT)/toolchains/llvm/prebuilt/linux-x86_64/bin/:$(PATH)" && \
        export ANDROID_NDK_HOME="$(ANDROID_NDK_ROOT)" && \
        ./Configure shared android-arm -D__ANDROID_API__=19 && \
        make -j2 SHLIB_VERSION_NUMBER= SHLIB_EXT=_1_1.so build_libs && \
        cd .. && \
        touch .openssl-arm
    openssl_arm.depends = extract_openssl
    QMAKE_EXTRA_TARGETS *= openssl_arm

    target.depends *= openssl_arm
    ANDROID_EXTRA_LIBS *= \
        $$OUT_PWD/openssl-arm/libcrypto_1_1.so \
        $$OUT_PWD/openssl-arm/libssl_1_1.so
}


# Build OpenSSL for 64bit ARM:
contains(ANDROID_ABIS, arm64-v8a) {
    openssl_arm64.target = .openssl-arm64
    openssl_arm64.commands = \
        cp -r $$SOURCE_DIR openssl-arm64 && \
        cd openssl-arm64 && \
        export CC=clang && \
        export ANDROID_API=21 && \
        export PATH="$(ANDROID_NDK_ROOT)/toolchains/llvm/prebuilt/linux-x86_64/bin/:$(PATH)" && \
        export ANDROID_NDK_HOME="$(ANDROID_NDK_ROOT)" && \
        ./Configure shared android-arm64 -D__ANDROID_API__=21 && \
        make -j2 SHLIB_VERSION_NUMBER= SHLIB_EXT=_1_1.so build_libs && \
        cd .. && \
        touch .openssl-arm64
    openssl_arm64.depends = extract_openssl
    QMAKE_EXTRA_TARGETS *= openssl_arm64

    target.depends *= openssl_arm64
    ANDROID_EXTRA_LIBS *= \
        $$OUT_PWD/openssl-arm64/libcrypto_1_1.so \
        $$OUT_PWD/openssl-arm64/libssl_1_1.so
}

# Build OpenSSL for 32bit Intel:
contains(ANDROID_ABIS, x86) {
    openssl_x86.target = .openssl-x86
    openssl_x86.commands = \
        cp -r $$SOURCE_DIR openssl-x86 && \
        cd openssl-x86 && \
        export CC=clang && \
        export ANDROID_API=19 && \
        export PATH="$(ANDROID_NDK_ROOT)/toolchains/llvm/prebuilt/linux-x86_64/bin/:$(PATH)" && \
        export ANDROID_NDK_HOME="$(ANDROID_NDK_ROOT)" && \
        ./Configure shared android-x86 -D__ANDROID_API__=19 && \
        make -j2 SHLIB_VERSION_NUMBER= SHLIB_EXT=_1_1.so build_libs && \
        cd .. && \
        touch .openssl-x86
    openssl_x86.depends = extract_openssl
    QMAKE_EXTRA_TARGETS *= openssl_x86

    target.depends *= openssl_x86
    ANDROID_EXTRA_LIBS *= \
        $$OUT_PWD/openssl-x86/libcrypto_1_1.so \
        $$OUT_PWD/openssl-x86/libssl_1_1.so
}

# Build OpenSSL for 64bit Intel:
contains(ANDROID_ABIS, x86_64) {
    openssl_x86_64.target = .openssl-x86_64
    openssl_x86_64.commands = \
        cp -r $$SOURCE_DIR openssl-x86_64 && \
        cd openssl-x86_64 && \
        export CC=clang && \
        export ANDROID_API=21 && \
        export PATH="$(ANDROID_NDK_ROOT)/toolchains/llvm/prebuilt/linux-x86_64/bin/:$(PATH)" && \
        export ANDROID_NDK_HOME="$(ANDROID_NDK_ROOT)" && \
        ./Configure shared android-x86_64 -D__ANDROID_API__=21 && \
        make -j2 SHLIB_VERSION_NUMBER= SHLIB_EXT=_1_1.so build_libs && \
        cd .. && \
        touch .openssl-x86_64
    openssl_x86_64.depends = extract_openssl
    QMAKE_EXTRA_TARGETS *= openssl_x86_64

    target.depends *= openssl_x86_64
    ANDROID_EXTRA_LIBS *= \
        $$OUT_PWD/openssl-x86_64/libcrypto_1_1.so \
        $$OUT_PWD/openssl-x86_64/libssl_1_1.so
}

}

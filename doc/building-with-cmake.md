# Building OpenTodoList using `cmake`

The following components are required to build OpenTodoList:

* Qt >= 5.10.0
* CMake >= 2.8.11 (optional)

Building using `cmake` is mostly the same on the various supported platforms. Open a terminal and run the following commands:


```bash
# First, clone the project:
git clone https://gitlab.com/rpdev/opentodolist.git

# Change into the project directory and create a shadow
# build directory:
cd opentodolist
mkdir build
cd build

# Configure the build:
cmake -DCMAKE_BUILD_TYPE=RELEASE ..

# Build the app:
make
```

Depending on your operating system and environment, you might have to
pass additional settings to `cmake`. For example, if you installed
Qt 5 into `/opt/qt510`, then you have to call it like this:


```bash
cmake -DCMAKE_BUILD_TYPE=RELEASE \
    -DCMAKE_PREFIX_PATH=/opt/qt510 ..
```

Please refer to the [Qt CMake manual](http://doc.qt.io/qt-5/cmake-manual.html)
for more information on using Qt with `cmake`.

Among others, you can set the following CMake parameters (via the `-D` switch)
to control the build and tweak the configuration of the app:

| **Parameter** | **Possible Values** | **Description** |
| ------------- | ------------------- | --------------- |
| `QTKEYCHAIN_STATIC` | ON, OFF | Control if the built in QtKeychain library is build statically or as shared library. |
| `OPENTODOLIST_WITH_UPDATE_SERVICE` | ON, OFF | Enable or disable the update check. |
| `OPENTODOLIST_WITH_APPIMAGE_EXTRAS` | ON, OFF | Enable extra functionality when building the app as AppImage. |
| `OPENTODOLIST_NEXTCLOUD_TEST_URL` | URL including username and password | The URL of a NextCloud instance. If given, unit tests will be run against this NextCloud instance. |
| `OPENTODOLIST_OWNCLOUD_TEST_URL` | URL including username and password | The URL of an ownCloud instance. If given, unit tests will be run against this ownCloud instance. |


## Building for Android

To build for Android, you need to have the Android SDK and NDK set up.
Then, call `cmake` passing it the required paths to your installations
plus the Android version and ABI you want to build for.

Examples:

```bash
mkdir build
cd build

# Configure for Android on ARM:
cmake \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_COMPILER:STRING=$ANDROID_NDK_ROOT/toolchains/arm-linux-androideabi-4.9/prebuilt/linux-x86_64/bin/arm-linux-androideabi-g++ \
    -DCMAKE_C_COMPILER:STRING=$ANDROID_NDK_ROOT/toolchains/arm-linux-androideabi-4.9/prebuilt/linux-x86_64/bin/arm-linux-androideabi-gcc \
    -DCMAKE_PREFIX_PATH:STRING=$QT_ARM_ROOT \
    -DQT_QMAKE_EXECUTABLE:STRING=$QT_ARM_ROOT/bin/qmake \
    -DCMAKE_SYSTEM_NAME=Android \
    -DCMAKE_SYSTEM_VERSION=16 \
    -DCMAKE_ANDROID_ARCH_ABI=armeabi-v7a \
    -DCMAKE_ANDROID_STL_TYPE=gnustl_shared \
    -DANDROID_SDK_ROOT=$ANDROID_SDK_ROOT \
    -DOPENTODOLIST_ANDROID_EXTRA_LIBS="$EXTRA_LIBS" \
    ..

# Configure for Android on x86:
cmake \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_COMPILER:STRING=$ANDROID_NDK_ROOT/toolchains/x86-4.9/prebuilt/linux-x86_64/bin/i686-linux-android-g++ \
    -DCMAKE_C_COMPILER:STRING=$ANDROID_NDK_ROOT/toolchains/x86-4.9/prebuilt/linux-x86_64/bin/i686-linux-android-gcc \
    -DCMAKE_PREFIX_PATH:STRING=$QT_X86_ROOT \
    -DQT_QMAKE_EXECUTABLE:STRING=$QT_X86_ROOT/bin/qmake \
    -DCMAKE_SYSTEM_NAME=Android \
    -DCMAKE_SYSTEM_VERSION=16 \
    -DCMAKE_ANDROID_ARCH_ABI=x86 \
    -DCMAKE_ANDROID_STL_TYPE=gnustl_shared \
    -DANDROID_SDK_ROOT=$ANDROID_SDK_ROOT \
    -DOPENTODOLIST_ANDROID_EXTRA_LIBS="$EXTRA_LIBS" \
    ..
```

**Note:** If you want to the app to be able to connect to servers via
secure channels (usually HTTPS), you must build OpenSSL for the desired
Android target first and pass the paths to the created `libcrypto.so` and
`libssl.so` - separated via a comma - via the
`OPENTODOLIST_ANDROID_EXTRA_LIBS` variable to `cmake`.

For more information about building for Android using cmake, refer to the
[CMake toolchain documentation](https://cmake.org/cmake/help/latest/manual/cmake-toolchains.7.html#cross-compiling-for-android).


## Known Issues

* The official Qt packages for Android you can download from the [qt.io](qt.io)
  are build against the Android NDK r10e, which is quite old (at least at the
  time of writing - where the most recent Qt release is 5.11.1). You better
  stick with this NDK to build the app for Android. Release APKs of OpenTodoList
  are build against r15c, so this NDK should work as well.

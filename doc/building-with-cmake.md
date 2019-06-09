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

Building for Android is supported via
[qt-cmake-android](https://gitlab.com/rpdev/qt-cmake-android). Basically,
you just need to make sure you expose the paths to the Android NDK and SDK
properly and provide the path to your Qt installation.


## Known Issues

* Building for Android requires at least Qt 5.12.3 as well as the Android
  NDK r19c or up. There is no support for older versions, even though it
  should be possible to build the app using lower versions. In particular,
  there is no dedicated support for using GCC for Android targets.

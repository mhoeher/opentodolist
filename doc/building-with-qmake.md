# Building OpenTodoList using `qmake`

The following components are required to build OpenTodoList:

* Qt >= 5.12.0

Building with `qmake` is the easiest way, especially as
`qmake` comes with Qt, which you anyway need.
The easiest way to build the app is opening the file
`opentodolist.pro` in QtCreator. Building on the
command line is also supported. The build is quite generic
for most supported platforms:


```bash
# First, clone the project:
git clone https://gitlab.com/rpdev/opentodolist.git

# Change into the project directory and create a shadow
# build directory:
cd opentodolist
mkdir build
cd build

# Configure the build
qmake CONFIG+=release \
    ..

# Build the app:
make
```

To further tweak the build, you can pass additional configuration switches
(i.e. `CONFIG+=XXX`) and variables (i.e. `VARIABLE=YYY`) to `qmake`.

The following configuration switches are supported:

| **Switch** | **Description** |
| ---------- | --------------- |
| `with_update_service` | Enable checking for new releases of the app. |
| `with_appimage_extras` | Enable special handling when building the app as AppImage. |


The following variables are supported:

| **Variable** | **Value** | **Description** |
| ------------ | --------- | --------------- |
| `NEXTCLOUD_URL` | URL including username and password. | The URL of a NextCloud server to run tests against. |
| `OWNCLOUD_URL` | URL including username and password. | The URL of an ownCloud server to run tests against. |


## Platform Specific Notes

If you plan to build for one of the platforms listed below,
please carefully review the notes given.


### Android

Most Android devices do not come with pre-installed OpenSSL
libraries. This means that when building the app, you won't
be able to connect to servers via the secure HTTPS protocol.
We strongly suggest you build OpenSSL libraries before building
OpenTodoList to be able to connect to servers in a secure way.

Building is quite easy, as you can reuse some scripts used
in the continuous integration system. Make sure you set up your
build environment properly. In particular ensure that the
`ANDROID_NDK_ROOT` environment variable points to your Android
NDK root folder. Then, depending on the target architecture you
plan to build for, run one of the following 
*from within the OpenTodoList root folder*:

```bash
# For armv7:
./ci/android/build-openssl \
    Setenv-android-armv7.sh \
    $PWD/pre-build/android/openssl/arm-linux-androideabi-4.9-api-18/

# For x86:
./ci/android/build-openssl \
    Setenv-android-x86.sh \
    $PWD/pre-build/android/openssl/x86-4.9-api-18/
```

Afterwards, you should be able to build the app as usual.

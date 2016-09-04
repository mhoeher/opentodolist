Creating Installers
===================

OpenTodoList comes with tools that allow to use the Qt Installer Framework (QtIFW)
to provide both online as well as offline installers. In the following, the
steps to create these installers are described per platform.

Linux
-----

Open a terminal and ensure, that all required tools are installed and visible in your
PATH environment variable. In particular,

* Qt (via the qmake executable)
* the Qt Installer Framework and
* patchelf
* Perl

are required. Usually, you want to have something like this done before the build:

    export PATH=/path/to/qt/bin:/path/to/qtinstallerframework/bin:$PATH

Now, we can proceed to build and package the app. Let's assume the source
code is in /home/rpdev/projects/opentodolist. In order to build the installer,
execute the following steps:

    cd /home/rpdev/projects
    mkdir build-opentodolist-release
    cd build-opentodolist-release
    cmake -DCMAKE_BUILD_TYPE=Release ../opentodolist
    make
    make test
    make LinuxInstaller

After these steps completed, you will find the installer
as well as the package repository for the installer in the
sub-directory installer_build. In order to create an official release (which
is hosted on www.rpdev.net), copy these files to 
https://www.rpdev.net/public/repositories/qtifw.


Mac OS X
--------

**None:**
Builds for Mac OS are currently not tested.

1. Build OpenTodoList
2. Run macdeployqt tool to copy Qt binaries into the app bundle:  
   **macdeployqt OpenTodoList.app -qmldir=/path/to/opentodolist/OpenTodoList/qml**
3. Create installers and online repository:  
    **perl $OTLSOURCEDIR/bin/mk-installer.pl 
     --installDir /path/to/OpenTodoList.app 
     --targetDir /path/to/where/installers/shall/be/saved 
     --os maces --arch ARCH
      --copy-qt-menu-nib --dmg**


Windows
-------

Open a terminal and ensure, that all required tools are installed and visible your
PATH environment variable. In particular,

* Qt (via the qmake executable)
* the Qt Installer Framework
* cmake

are required. Usually, you want to have something like this done before the build:

    set PATH=C:\path\to\qt\bin;C:\path\to\qtinstallerframework\bin;C:\path\to\cmake\bin;$PATH

Now, we can proceed to build and package the app. Let's assume the source
code is in C:\opentodolist. In order to build the installer,
execute the following steps:

    C:
    mkdir build-opentodolist-release
    cd build-opentodolist-release
    cmake -DCMAKE_BUILD_TYPE=Release ../opentodolist
    make
    make test
    make WindowsInstaller

After these steps completed, you will find the installer
as well as the package repository for the installer in the
sub-directory installer_build. In order to create an official release (which
is hosted on www.rpdev.net), copy these files to 
https://www.rpdev.net/public/repositories/qtifw.

**Note:**
OpenTodoList for Windows can be build using [MXE](http://mxe.cc/) on a Linux host.
Download MXE and build a Qt
installation by running the following commands in the MXE directory:

    make \
        MXE_TARGETS="x86_64-w64-mingw32.shared i686-w64-mingw32.shared" \
        qtbase qtdeclarative qtgraphicaleffects \
        qtimageformats qtlocation qtmultimedia \
        qtquickcontrols qtquickcontrols2 qtsensors \
        qtservice qtsvg qttranslations qtwinextras \
        qtxmlpatterns qttools

This will build both a 32bit as well as a 64bit version of Qt. To build
against these, run the following (assuming Windows 64bit as target):

    cd /home/rpdev/projects
    mkdir build-opentodolist-windows-64bit-release
    /path/to/mxe/usr/bin/x86_64-w64-mingw32.shared-cmake \
        -DCMAKE_BUILD_TYPE=Release ../OpenTodoList
    make
    make test
    make WindowsInstaller

Note that you have to include the path to the **Windows vesion of the
Qt Installer Framework** in your *PATH* environment variable for the installer
build to succeed. Replace *x86_64-w64-mingw32.shared-cmake* with
*i686-w64-mingw32.shared-cmake* if you wish to build the 32bit version of the
app.



Android
-------

Building for Android requires the Android SDK as well as the Android NDK. Furthermore,
the [ECM](https://api.kde.org/ecm/) moduleis required which provides an
[Android toolchain file](https://api.kde.org/ecm/toolchain/Android.html) for Cmake.
Assume the sources of the app are in /home/rpdev/projects/opentodolist, run the following
commands to build the app:

    export ANDROID_NDK=/path/to/android/ndk
    export JAVA_HOME=/usr/lib/jvm/java-openjdk # for OpenJDK
    cd /home/rpdev/projects
    mkdir build-opentodolist-android-release
    cd build-opentodolist-android-release
    cmake \
        -DCMAKE_TOOLCHAIN_FILE=/usr/share/ECM/toolchain/Android.cmake \
        -DANDROID_SDK_ROOT=/path/to/android/sdk \
        -DANDROID_NDK=/path/to/android/ndk \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_PREFIX_PATH=/path/to/qt/for/android/installation \
        -DANDROID_APK_DIR=../opentodolist/app/android \
        -DCMAKE_INSTALL_PREFIX=$PWD/android_install \
        -DQTANDROID_EXPORTED_TARGET=OpenTodoList \
        ../opentodolist
    make
    make create-apk-OpenTodoList

The APK will be stored in *OpenTodoList_build_apk/bin*. To install it onto
a device, run:

    /path/to/android-sdk/platform-tools/adb install -r \
        ./OpenTodoList_build_apk/bin/QtApp-debug.apk

To create the release APK, run the following:

    cp ./OpenTodoList_build_apk/bin/QtApp.ap_ \
        ./OpenTodoList_build_apk/bin/OpenTodoList.apk
    jarsigner -verbose -sigalg SHA1withRSA -digestalg SHA1 \
        -keystore path/to/android_release.keystore 
        ./OpenTodoList_build_apk/bin/OpenTodoList.apk aliasname
    zipalign -v 4 OpenTodoList_build_apk/bin/OpenTodoList.apk \
        OpenTodoList_build_apk/bin/OpenTodoList-release-aligned.apk

Replace the path to your keystore and specify a proper `aliasname`.

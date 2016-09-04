Build Instructions
==================

This guide describes how to build the application from the sources.



Building for Your Host Operating System
---------------------------------------

Make sure you install the following requirements:

* Qt 5 (including development files)
* C++ compiler (must be C++11 compliant)
* GNU Make
* CMake

Take care that the tools can be found by having their installation paths in your `PATH` environment
variable. Building is a matter of the following commands:

    mkdir build
    cd build
    cmake -DCMAKE_BUILD_TYPE=Release ..
    make

You might want to build against a custom installation of Qt (e.g. if you work on Linux with Qt
being installed from your distributions repository and you have a custom version of Qt installed
via the Qt installer). In this case, the sequence looks slightly different. For example, to
build against a custom Qt 5.7 installation for x86 hosts:

    mkdir build
    cd build
    cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=/path/to/Qt/5.7/gcc_64 ..
    make

Building for Windows using MXE
------------------------------

The [M Cross Environment](http://mxe.cc/) provides a way to easily build Windows executable
from a Linux host. To get it, simply use an

    git clone https://github.com/mxe/mxe.git

Now you need to build Qt for Windows in that environment. In a terminal, run the following
commands to build the required prerequisites:

    cd mxe
    # Build 32 bit Windows libraries:
    make \
        MXE_TARGETS="i686-w64-mingw32.shared" \
        qtbase qtdeclarative qtgraphicaleffects \
        qtimageformats qtlocation qtmultimedia \
        qtquickcontrols qtquickcontrols2 qtsensors \
        qtservice qtsvg qttranslations qtwinextras \
        qtxmlpatterns qttools
    # Build 64 bit Windows executables:
    make \
        MXE_TARGETS="x86_64-w64-mingw32.shared" \
        qtbase qtdeclarative qtgraphicaleffects \
        qtimageformats qtlocation qtmultimedia \
        qtquickcontrols qtquickcontrols2 qtsensors \
        qtservice qtsvg qttranslations qtwinextras \
        qtxmlpatterns qttools

Building Qt this way will take some time. When the build is done, you can cross compile the
application using the special cmake executable in the MXE directory. Change into the
application directory and use the following commands:
    
    # Build 64 bit Windows binaries:
    mkdir build-windows-64bit
    cd build-windows-64bit
    /path/to/mxe/usr/bin/x86_64-w64-mingw32.shared-cmake \
        -DCMAKE_BUILD_TYPE=Release ..
    make
    cd ..
    # Build 32 bit Windows binaries:
    mkdir build-windows-32bit
    cd build-windows-32bit
    /path/to/mxe/usr/bin/i686-w64-mingw32.shared-cmake \
        -DCMAKE_BUILD_TYPE=Release ..
    make
    cd ..


Building for Android
--------------------

To build for Android, you need - in addition to the usual tools - the following:

* A Java runtime (e.g. OpenJDK)
* Ant
* Qt for Android (in the target architecture you desire, e.g. ARM or x86).
* [Android SDK](https://developer.android.com/studio/index.html#downloads)
* [Android NDK](https://developer.android.com/ndk/index.html)
* [ECM](https://api.kde.org/ecm/manual/ecm.7.html)

To build an APK issue the following commands:

    export ANDROID_SDK=/path/to/android/sdk # Installation path of SDK
    export ANDROID_NDK=/path/to/android/ndk # Installation path of NDK
    export JAVA_HOME=/usr/lib/jvm/java-openjdk # e.g. for OpenJDK
    export QT_HOME=/path/to/qt/android_armv7 # Path to Qt to be used, e.g. Qt for ARMv7
    mkdir build
    cd build
    cmake \
        -DCMAKE_TOOLCHAIN_FILE=/usr/share/ECM/toolchain/Android.cmake \
        -DANDROID_SDK_ROOT=$ANDROID_SDK \
        -DANDROID_NDK=$ANDROID_NDK \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_PREFIX_PATH=$QT_HOME \
        -DANDROID_APK_DIR=../app/android \
        -DCMAKE_INSTALL_PREFIX=$PWD/android_install \
        -DQTANDROID_EXPORTED_TARGET=OpenTodoList \
        ..
    make
    make create-apk-OpenTodoList

This should build the app for the target platform of the Qt version the QT_HOME
variable points to. The APK is stored in `build/android_install`. To install it to
a target device use

    $ANDROID_SDK/platform-tools/adb install -r \
        ./OpenTodoList_build_apk/bin/QtApp-debug.apk

Note that this is a debug version of the app. To build a release one, you need to run these
additional commands:

    cp ./OpenTodoList_build_apk/bin/QtApp.ap_ \
        ./OpenTodoList_build_apk/bin/OpenTodoList.apk
    jarsigner -verbose -sigalg SHA1withRSA -digestalg SHA1 \
        -keystore /path/to/android_release.keystore 
        ./OpenTodoList_build_apk/bin/OpenTodoList.apk aliasname
    zipalign -v 4 OpenTodoList_build_apk/bin/OpenTodoList.apk \
        OpenTodoList_build_apk/bin/OpenTodoList-release-aligned.apk

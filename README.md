# OpenTodoList - A Todo and Task Management Application

## About

OpenTodoList is a todo list and note taking application. Organize todo lists,
notes and images in libraries, which can be stored either completely local
on the device you are working on (and hence ensure no information leaks out
to untrusted third parties) or use the built in synchronization features which
allows you to synchronize your libraries across devices using your self-hosted
NextCloud or ownCloud server (or other WebDAV servers). In addition, a library
is just a directory holding the items of your library as simple files - this
allows you to use any kind of third party synchronization tool (like DropBox)
to sync your information.


## License

This program is free software: you can redistribute it and/or modify
it under the terms of the
[GNU General Public License](https://www.gnu.org/licenses/gpl-3.0-standalone.html)
as published by the Free Software Foundation, either version 3 of the License,
or (at your option) any later version.


## Download

Pre-build packages of the app are hosted on
[GitHub](https://github.com/mhoeher/opentodolist/releases). In addition,
the app is available in the following app stores:


[<img src="https://play.google.com/intl/en_us/badges/images/apps/en-play-badge.png" width="250"/>](https://play.google.com/store/apps/details?id=net.rpdev.opentodolist)


## Get Help

If you need help with the app, just file an issue in the
[Bug Tracker](https://gitlab.com/rpdev/opentodolist/issues).


## Report Bugs

OpenTodoList is still a very young project which is on top developed as a freetime
project. Hence, please bear with us if the app is not yet working perfectly fine.
In case you found an issue, please head over to the [GitLab issue tracker
of OpenTodoList](https://gitlab.com/rpdev/opentodolist/issues) and file a
bug report there. Also make use of this if you want to request a new feature.


# Supported Operating Systems

OpenTodoList is developed in C++ on top of the Qt 5 framework. In theory that
means, that you should be able to use the app on a wide range of operating
systems and devices. Practically, the app is regularly tested and build on
the following platforms:

* Linux.
* Windows (32 and 64 bit, using [MXE](http://mxe.cc/) and Visual Studio).
* macOS.
* Android (ARM and x86).

If a system is not listed above, this does not mean that the app does not work
on that platform, but just that we've not tries yet (usually because we do not
have the resources to do so). As OpenTodoList is open source, you can easily
give it a try and check if you can get it to work your your platform of
choice.


## Build Requirements and Instructions

The following components are required to build OpenTodoList:

* Qt >= 5.10.0
* CMake >= 2.8.11 (optional)

The project uses `qmake` and `cmake` as a build system, which work the same way on more
or less all platforms:

Using `qmake`:

```bash
git clone https://gitlab.com/rpdev/opentodolist.git
cd opentodolist
mkdir build
cd build
qmake CONFIG+=release ..
make
```

Using `cmake`:

```bash
git clone https://gitlab.com/rpdev/opentodolist.git
cd opentodolist
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=RELEASE ..
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


**Note:** Currently, the following target platforms do not support compilation
via `cmake`:

* Android

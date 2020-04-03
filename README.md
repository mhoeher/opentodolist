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


## Download 

### Android

The app is available on Android. You can get it on the following stores:


<a href='https://play.google.com/store/apps/details?id=net.rpdev.opentodolist'><img alt='Get it on Google Play' src='https://play.google.com/intl/en_us/badges/static/images/badges/en_badge_web_generic.png' height='40'/></a>
<a href='https://apt.izzysoft.de/fdroid/index/apk/net.rpdev.opentodolist'><img alt='Get it on IzzyOnDroid' src='https://gitlab.com/IzzyOnDroid/repo/-/raw/master/assets/IzzyOnDroid.png?inline=false' height='40'/></a>

To add the IzzyOnDroid F-Droid repository to your device, follow the instructions [here](https://apt.izzysoft.de/fdroid/).


### iOS

The app is available on the iOS App Store:

<a href="https://apps.apple.com/us/app/opentodolist/id1490013766?mt=8"><img src="https://linkmaker.itunes.apple.com/en-gb/badge-lrg.svg?releaseDate=2020-01-02&kind=iossoftware&bubble=ios_apps" width='135px' height='40'/></a>


### Desktop Systems

Pre-build packages of the app for Linux, macOS and Windows are hosted on [GitHub](https://github.com/mhoeher/opentodolist/releases).

Additionally, you can find the app on the following stores:

<a href="https://snapcraft.io/opentodolist">
<img alt="Get it from the Snap Store" src="https://snapcraft.io/static/images/badges/en/snap-store-black.svg" height="40"></a>
<a href='https://flathub.org/apps/details/net.rpdev.OpenTodoList'><img height='40' alt='Download on Flathub' src='https://flathub.org/assets/badges/flathub-badge-en.png'/></a>



## License

This program is free software: you can redistribute it and/or modify
it under the terms of the
[GNU General Public License](https://www.gnu.org/licenses/gpl-3.0-standalone.html)
as published by the Free Software Foundation, either version 3 of the License,
or (at your option) any later version.


## Get Help

If you need help with the app, just file an issue in the
[Bug Tracker](https://gitlab.com/rpdev/opentodolist/issues).


## Report Bugs

OpenTodoList is still a very young project which is on top developed as a freetime
project. Hence, please bear with us if the app is not yet working perfectly fine.
In case you found an issue, please head over to the [GitLab issue tracker
of OpenTodoList](https://gitlab.com/rpdev/opentodolist/issues) and file a
bug report there. Also make use of this if you want to request a new feature.


# Help Translating the App

If you like using the app, you can help to make it even better by helping
us to translate it to other languages. Translation can be done easily
via [poeditor.com](). Like the idea?
Then [join the project](https://poeditor.com/join/project/ztvOymGNxn).


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

First, get the sources of the app, for example by cloning it from GitLab:

```bash
git clone --recurse-submodules https://gitlab.com/rpdev/opentodolist.git
```

**Important:** Make sure you clone recursively. The app requires some
libraries that are included in source form. If you do not clone
recursively, the build will likely fail. If you already cloned the repository,
you can run

```bash
git submodule update --init
```

to clone the submodules. You also might want to run this command regularly
after pulling new changes.

The application is mainly build using `qmake`. There still is some support
for `cmake`, but it is currently not maintained.

* Continue with [Build Instructions using `qmake`](./doc/building-with-qmake.md)
* Continue with [Build Instructions using `cmake`](./doc/building-with-cmake.md)



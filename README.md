# OpenTodoList - A Todo and Task Management Application

## About

OpenTodoList is a todo list and note taking application. Organize todo lists,
notes and images in libraries, which can be stored either completely local
on the device you are working on (and hence ensure no information leaks out
to untrusted third parties) or use the built in synchronization features which
allows you to synchronize your libraries across devices. Currently, the following
backend servers are supported for synchronization:

* NextCloud
* ownCloud
* Generic WebDAV servers - note that due to limitations of some servers,
  the sync might not be working optimally or at all, this is a limitation of the
  server that the app cannot do anything about. Feel free to raise an issue in
  case you think your server is still capable to run a sync properly 😉
* Dropbox

In addition, a library
is just a directory holding the items of your library as simple files - this
allows you to use any kind of third party synchronization tool (like DropBox)
to sync your information.

Currently, the app is available on the following platforms:

* Android
* iOS
* Linux (pre-build as AppImage, via Flatpak and Snap)
* macOS
* Windows

The app is developed on top of the Qt framework - so even if a system is not
listed explicitly above, you might still be able to compile the app for it on
your own.

<a
    href="doc/screenshots/Android Tablet/library.png"
    alt="Libraries are used to organize items."
    ><img src="doc/screenshots/Android Tablet/library.png" height="300px"
/></a>
<a
    href="doc/screenshots/Android Tablet/todolist.png"
    alt="Todo lists collect todos - you can have arbitrary many todo lists per library."
    ><img src="doc/screenshots/Android Tablet/todolist.png" height="300px"
/></a>
<a
    href="doc/screenshots/Android Tablet/todo.png"
    alt="Todos are single tasks that can be marked as done - they can be further broken down into tasks."
    ><img src="doc/screenshots/Android Tablet/todo.png" height="300px"
/></a>
<a
    href="doc/screenshots/Android Tablet/note.png"
    alt="Notes are used to collect text - you can add additional pages to each note, making them notebooks."
    ><img src="doc/screenshots/Android Tablet/note.png" height="300px"
/></a>
<a
    href="doc/screenshots/Android Tablet/image.png"
    alt="Images can be added to libraries as well - you can add additional text to them, too."
    ><img src="doc/screenshots/Android Tablet/image.png" height="300px"
/></a>

## Download

### Android

The app is available on Android. You can get it on the following stores:


<a href='https://play.google.com/store/apps/details?id=net.rpdev.opentodolist'><img alt='Get it on Google Play' src='https://play.google.com/intl/en_us/badges/static/images/badges/en_badge_web_generic.png' height='40'/></a>
<a href='https://apt.izzysoft.de/fdroid/index/apk/net.rpdev.opentodolist'><img alt='Get it on IzzyOnDroid' src='https://gitlab.com/IzzyOnDroid/repo/-/raw/master/assets/IzzyOnDroid.png?inline=false' height='40'/></a>

To add the IzzyOnDroid F-Droid repository to your device, follow the instructions [here](https://apt.izzysoft.de/fdroid/).


### iOS

The app is available on the iOS App Store:

[<img src="https://developer.apple.com/assets/elements/badges/download-on-the-app-store.svg"
alt="Demo of the Nextcloud iOS files app"
height="40">](https://apps.apple.com/us/app/opentodolist/id1490013766)



### Desktop Systems

Pre-build packages of the app for Linux, macOS and Windows are hosted on [GitHub](https://github.com/mhoeher/opentodolist/releases).

Additionally, you can find the app on the following stores:

<a href="https://snapcraft.io/opentodolist">
<img alt="Get it from the Snap Store" src="https://snapcraft.io/static/images/badges/en/snap-store-black.svg" height="40"></a>
<a href='https://flathub.org/apps/details/net.rpdev.OpenTodoList'><img height='40' alt='Download on Flathub' src='https://flathub.org/assets/badges/flathub-badge-en.png'/></a>
<a href='https://aur.archlinux.org/packages/opentodolist/'><img height='40' alt='Install from AUR' src='assets/AUR.png'/></a>



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
* Windows.
* macOS.
* Android (ARM and x86).
* iOS.

If a system is not listed above, this does not mean that the app does not work
on that platform, but just that we've not tried yet (usually because we do not
have the resources to do so). As OpenTodoList is open source, you can easily
give it a try and check if you can get it to work your your platform of
choice.


## Build Requirements and Instructions

Please refer to the [Build Instructions](./doc/build-instructions.md) to learn
how to build the app.

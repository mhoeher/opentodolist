OpenTodoList - A Todo and Task Management Application
=====================================================

About
-----

OpenTodoList is an open source task managing and note taking application.
It allows to create arbitrary many libraries, which are stored as directories on
the local storage. Inside each library, todo lists, notes and images can be created.
Todo lists in turn consist of todos (which can also have sub-tasks).

As data is by default stored locally, you remain in full control of your data.
However, if you need to securely sync libraries between multiple of your devices,
you can use any service of your choice to do so. For example, if you have set
up an ownCloud instance, you can use the ownCloud desktop client to sync your library
between your devices and the cloud.

License
-------

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Download
--------

### Linux and Windows

You can find installers for various operating systems on the projects
[homepage](http://www.rpdev.net/home/project/opentodolist).

### Android

[<img src="https://play.google.com/intl/en_us/badges/images/apps/en-play-badge.png" width="250"/>](https://play.google.com/store/apps/details?id=net.rpdev.opentodolist)


Get Help
--------

If you need some help with OpenTodoList, please visit us on [Google+
    ](https://plus.google.com/communities/106580508283710923527).

Report Bugs
-----------

OpenTodoList is still a very young project which is on top developed as a freetime
project. Hence, please bear with us if the app is not yet working perfectly fine.
In case you found an issue, please head over to the [GitLab issue tracker
of OpenTodoList](https://gitlab.com/rpdev/opentodolist/issues) and file a
bug report there. Also make use of this if you want to request a new feature.


Building
--------

OpenTodoList is written on top of Qt 5 and QML. You need at least Qt 5.7
(compilation with earlier versions might succeed, but running the
application might fail in this case). To get the latest sources, you furthermore
require git. To get get the sources and change into the project directory, use the
following commands:

    git clone https://gitlab.com/rpdev/opentodolist.git
    cd opentodolist

To configure and compile the project, you will need a working C++ compiler as well
as [CMake](https://cmake.org/). Depending on your operating system other tools might
be required as well. Most commonly, you will need something like
[GNU Make](https://www.gnu.org/software/make/). In the simplest case, building
might be a matter of issuing the following commands:

    mkdir ./build
    cd ./build
    cmake -DCMAKE_BUILD_TYPE=Release ..
    make

That's it. You should find a runnable version of the application in `./build/app`. To build
for other platforms (especially when cross-compiling), please also refer to the 
detailed [Build instructions](./doc/building.md).


Supported Operating Systems
---------------------------

OpenTodoList can be run on a variety of operating systems. It is regularly build
and tested on the following systems:

* Linux
* MS Windows 10
* Android (ARMv7, x86)

If an OS is not mentioned in the above list, that does not mean that OpenTodoList won't
compile and run there. Due to the underlying Qt libraries, chances are good that
your OS is supported as well. Just follow the above build instructions.

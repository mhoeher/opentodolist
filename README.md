OpenTodoList - A Todo and Task Management Application
=====================================================

About
-----

OpenTodoList is an open source todo and task management application.
It allows creation of arbitrary many todo lists. Todo lists can in turn
contain todos. A todo has various attributes, such as a title, done state,
due date and a priorty. Furthermore, they have a description and can have
arbitrary many tasks.

Download
--------

###Linux, Mac OS X and Windows

You can find installers for various operating systems on the projects
[homepage](http://www.rpdev.net/home/project/opentodolist).

###Android

OpenTodoList is available from the Google Play Store for Android devices. However,
due to its early development state, it is currently available as a beta only. In
order to give it a try, follow these steps:

1. Join the [OpenTodoList community on 
   Google+](https://plus.google.com/communities/106580508283710923527).
2. Opt-in for receiving beta updates by visiting the appropriate [Google
    Play site](https://play.google.com/apps/testing/net.rpdev.opentodolist).
3. On any device where you want to install the app, visit its [Google
   Play Store entry](https://play.google.com/store/apps/details?id=net.rpdev.opentodolist).

Get Help
--------

If you need some help with OpenTodoList, please visit us on [Google+
    ](https://plus.google.com/communities/106580508283710923527).

Building
--------

OpenTodoList is written on top of Qt 5 and QML. You need at least Qt 5.4
(compilation with earlier versions might succeed, but running the
application might fail in this case). To get the latest sources, you furthermore
require git. To download and build the application, run the following steps:

    git clone https://gitorious.org/opentodolist/opentodolist.git
    mkdir build-opentodolist
    cd build-opentodolist
    qmake ../opentodolist CONFIG+=release
    make

Available Backends
------------------

OpenTodoList implements a plugin mechanism for its storage backends. A backend
is any kind of *service* that allows you to store todos. Right now, the following
backends are implemented:

###Todos in local XML files

This backend stores your todos locally in a set of XML files. This makes it
an ideal choice for sensible data that you do not want to store "in the cloud".

Supported Operating Systems
---------------------------

OpenTodoList can be run on a variety of operating systems. It is regularly build
and tested on the following systems:

* Linux
* Mac OS X 10.10 and up
* MS Windows 8.1
* Android (ARM, ARMv7, x86)

If an OS is not mentioned in the above list, that does not mean that OpenTodoList won't
compile and run there. Due to the underlying Qt libraries, chances are good that
your OS is supported as well. Just follow the above build instructions.

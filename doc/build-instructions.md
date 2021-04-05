# Build Instructions

This document describes the required components and commands needed to build OpenTodoList.


## Getting The Source

First, get the sources of the app, for example by cloning it from GitLab:

```bash
git clone --recurse-submodules https://gitlab.com/rpdev/opentodolist.git
```

**Important:** Make sure you clone recursively. The app requires some libraries that are included in source form. If you do not clone recursively, the build will likely fail. If you already cloned the repository, you can run

```bash
git submodule update --init --recursive
```

to clone the submodules. You also might want to run this command regularly after pulling new changes.

By default, there are two branches in the repository:

- The `development` branch is where the actual development happens. This is the default branch which is active when cloning the project. As there are quite some tests ongoing, it usually is *safe* using that branch. As new features and fixes first go into this branch, it is the right choice if you want to have the latest and greatest available feature set.
- In addition, there is a `stable` branch. This branch is updated whenever there is a new *release*, i.e. a new version of the app is released from the `development` branch. Usually, the `stable` branch is not updated between releases except for important bug fixes.

In addition, whenever a release of the app happens, a new tag is created.

To switch between the different branches or tags, use the `git checkout` command like this:

```bash
# Switch to the stable branch:
git checkout stable
git submodule update --init --recursive

# Switch to a specific tag, e.g. v3.33.0:
git checkout 3.33.0
git submodule update --init --recursive

# Switch back to the development branch:
git checkout development
git submodule update --init --recursive
```

Please note the `git submodule` command: It is necessary as between branches or releases the concrete versions of included sub-modules might change.


## Required Software Components

OpenTodoList is built on top of the [Qt](qt.io) framework. In addition, the app requires several additional libraries to cover functionality which is not present in Qt. Last but not least, there are also optional dependencies which are not strictly required for building, but which - if present - enhance the functionality of the app.

Most of the required add-on libraries are included as `git submodule`s, so usually, all you need to do is make sure you have a working Qt development installation. You then can open the project in Qt Creator and build it right from there. However, building everything from scratch takes quite some time, so depending on the target platform you want to build for, you might be better off building against system provided libraries.

Long story short, here is the list of libraries that are needed to build the app:


### Strictly Required

- [Qt](qt.io) >= 5.14. More specifically, the following Qt modules are used:
  - Base.
  - Declarative.
  - Remote Objects.
  - Quick Controls 2.
- `qmake` (it comes usually pre-installed with Qt) **or** [`cmake`](https://cmake.org/) >= 3.5.
- A suitable compiler.
- `make`, `ninja-build` or another suitable build tool.


### Strictly Required But Included In Source

The following components are strictly required to build the app and hence we include them in source form in the `3rdparty` folder. However, if you already have a built version of these, you can build against these pre-build versions:

- KDE's [`syntax-highlighting`](https://api.kde.org/frameworks/syntax-highlighting/html/index.html) framework >= 5.70.0.
- [QLMDB](https://gitlab.com/rpdev/qlmdb) >= 1.2.0.
- [qt-keychain](https://github.com/frankosterfeld/qtkeychain/) >= 0.10.0.
- [SynqClient](https://gitlab.com/rpdev/synqclient) >= 1.4.0.


### Optional And Alternative Dependencies

The following are optional dependencies or ones that the app can be build against instead of one of the *default* dependencies. They are not required at all, but usually will improve the user experience:

- KDE's [`KNotifications`](https://api.kde.org/frameworks/knotifications/html/index.html) library.


### A Note On Versions

We will try to keep the list of dependencies and their required versions up to date. However, if you encounter issues building the app, check if an update to one of the dependent libraries is available.

Also note that it might be possible to build against older versions of the listed dependencies. However, the versions noted above are the ones that are tested on a regular base in the app's CI/CD pipelines.


### Installing Dependencies

Installing the above mentioned dependencies is very much specific to the platform you want to build for. If you decide to download the Qt SDK from the official website, you should have most of the required tools at hand. As the app includes additional dependencies in source form, you then should be able to fire up Qt Creator, open the app and build it without further issues.

On Linux, thanks to the availability of package managers, getting the required libraries is usually pretty simple. For example...


#### Fedora Linux

On Fedora, run the following to install all the pre-requisites required to build the app:

```bash
dnf install -y \
    make \
    cmake \
    gcc \
    g++ \
    qt5-{qtbase,qtdeclarative,qtremoteobjects,qtquickcontrols2}-devel \
    qt5-linguist \
    qtkeychain-qt5-devel \
    kf5-syntax-highlighting-devel \
    kf5-knotifications-devel
```


## Building The App

To build the app, you have several choices:


### Build Using Qt Creator

To build OpenTodoList, you could simply open the sources with Qt Creator. This should work out-of-the-box for most targets. It will - by default - build the app against the versions of required libraries that are included in source form. If you want to change this or otherwise tweak the build, read on to learn about the available build options.


### Build Using `cmake`

You can build and install the app using `cmake` on the command line. If you are familiar with `cmake`, the following instructions should be pretty familiar:

```bash
# Change into the source folder:
cd path/to/opentodolist

# Create a build directory:
mkdir build
cd build

# Configure the project:
cmake -DCMAKE_BUILD_TYPE=Release ..

# Build it:
cmake --build .

# And install it (might require running with `sudo`):
cmake --build . --target install
```

There are several options available that you can pass to `cmake` when configuring the project (via `-D<OPTION_NAME>=<OPTION_VALUE>`). In particular, these are:

| **Option** | **Default Value** | **Description** |
| ---------- | ----------------- | --------------- |
| `OPENTODOLIST_WITH_UPDATE_SERVICE` | `OFF` | Can be used to enable built-in update checks. This will cause the app to regularly check if a new version of OpenTodoList is available. |
| `OPENTODOLIST_WITH_KNOTIFICATIONS` | `OFF` | Use `KNotifications` to show the system tray icon and display notifications. The user experience is way cleaner than what the Qt built-in functionality provides. |
| `OPENTODOLIST_USE_SYSTEM_LIBRARIES` | `OFF` | Use system versions of required 3rd party libraries instead of building them from source. The default is off. This option is mostly useful if you want to package the app e.g. for distribution via a package manager or if you build it locally and want to avoid long compilation times by re-using pre-built versions of the required modules. |
| `OPENTODOLIST_USE_SYSTEM_QTKEYCHAIN` | `${OPENTODOLIST_USE_SYSTEM_LIBRARIES}` | Use a system version of the QtKeychain library instead of building it from source. |
| `OPENTODOLIST_USE_SYSTEM_KF_SYNTAX_HIGHLIGHTING` | `${OPENTODOLIST_USE_SYSTEM_LIBRARIES}` | Use a system version of KDE's KSyntaxHighlighting library instead of building it from source. |
| `OPENTODOLIST_USE_SYSTEM_QLMDB` | `${OPENTODOLIST_USE_SYSTEM_LIBRARIES}` | Use a system version of the QLMDB library instead of building it from source. |
| `OPENTODOLIST_USE_SYSTEM_SYNQCLIENT` | `${OPENTODOLIST_USE_SYSTEM_LIBRARIES}` | Use a system version of the SynqClient library instead of building it from source. |

**Note:** There are additional options available, which allow fine tuning for specific targets. Please refer to the various build scripts in the top-level `ci` folder to learn more. However, usually, there is no reason to use them.


### Build using `qmake`

`qmake` is the default build system of Qt itself until version 5. Starting with Qt 6, the Qt project migrated to `cmake` as their default build system. `cmake` is far more powerful, so we recommend using it to also build OpenTodoList if possible. However, currently we also maintain a `qmake` based build, mostly because using Qt 5 not all supported platforms can be built for easily using `cmake`.

The `qmake` based build is pretty similar to the `cmake` based one:

```bash
# Change into the source folder:
cd path/to/opentodolist

# Create a build directory:
mkdir build
cd build

# Configure the project:
qmake CONFIG+=release ..

# Build it:
make -j$(nproc)

# And install it (might require running with `sudo`):
make install
```

**Note:** The `qmake` based build is in *maintenance* mode. We will keep it working as long as required, but some new features will only be available with `cmake` over time.
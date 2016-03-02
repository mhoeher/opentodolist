Creating Installers
===================

OpenTodoList comes with tools that allow to use the Qt Installer Framework (QtIFW)
to provide both online as well as offline installers. In the following, the
steps to create these installers are described per platform.

Linux
-----

Open a terminal and ensure, that all required tools are installed and visible your
PATH environment variable. In particular,

* Qt (via the qmake executable)
* the Qt Installer Framework and
* patchelf

are required. Usually, you want to have something like this done before the build:

    export PATH=/path/to/qt/bin:/path/to/qtinstallerframework/bin:$PATH

Now, we can proceed to build and package the app. Let's assume the source
code is in /home/rpdev/projects/opentodolist. In order to build the installer,
execute the following steps:

    cd /home/rpdev/projects
    mkdir build-opentodolist-release
    cd build-opentodolist-release
    qmake CONFIG+=release ../opentodolist
    make
    make check
    make installer_linux_x86_64

After these steps completed, you will find the online and offline installers
as well as the package repository for the online installer in the
sub-directory installer_build. In order to create an official release (which
is hosted on www.rpdev.net), copy these files to 
https://www.rpdev.net/public/repositories/qtifw.


Mac OS X
--------

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

1. Build OpenTodoList
2. Copy the directories for distribution (bin, share) from the build
   directory into a directory **C:\path\to\dist\OpenTodoList**
   *Note:* Mind the trailing OpenTodoList directory. It is required for the
   subsequent scripts to succeed.
3. Run windeployqt.exe on the OpenTodoList.exe
4. Change into **C:\path\to\dist**.
4. Create installers and online repository:  
     **perl %OTLSOURCEDIR%\bin\mk-installer.pl 
     --installDir . 
     --targetDir C:\path\to\installersdir 
     --os windows --arch ARCH**

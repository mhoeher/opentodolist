Creating Installers
===================

OpenTodoList comes with tools that allow to use the Qt Installer Framework (QtIFW)
to provide both online as well as offline installers. In the following, the
steps to create these installers are described per platform.

Linux
-----

1. Build OpenTodoList
2. Install it using:  
    **make install $INSTALLROOT=$INSTALLROOT/OpenTodoList**  
   Note that *$INSTALLROOT* is an arbitrary location. You
   must install however into another subdirectory called
   OpenTodoList for the other scripts to work properly.
3. Deploy Qt libraries:  
    **perl $OTLSOURCEDIR/bin/linux-deploy-qt.pl
    --qt /path/to/qt --exe $INSTALLROOT/OpenTodoList/bin/OpenTodoList**
4. Create installers and online repository:  
    **perl $OTLSOURCEDIR/bin/mk-installer.pl
        --installDir $INSTALLROOT
        --targetDir /path/to/where/installers/shall/be/saved
        --os linux --arch ARCH**

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

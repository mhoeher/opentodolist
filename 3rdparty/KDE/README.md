# Update Notice

In case you update one of the included libraries, please mind the following.


## Syntax Highlighting

For the sake of simplicity, we compile the sources of the KDE Syntax Highlighting library directly
into the OpenTodoList main library. This ensures that we can easily compile for any target platform
Qt runs on without depending on CMake (which is - at the time of writing this - sometimes a real
pain to deal with in case of some target platforms like Android and iOS).

Compiling the actual sources is not the big deal, but the library uses some generated assets we
need to include manually. This is done by the script
`../../bin/update-kde-syntax-highlighting-assets.sh`. This script needs to be re-run after updating
the Syntax Highlighting sub-module. For this purpose, make sure you are in a shell where you have:

* A suitable Qt version
* ECM
* CMake

available as well as usual UNIX command line tools.

#!/bin/bash

set -e

# Enable ccache:
ln -s "$(which i686-w64-mingw32-g++)" /usr/local/bin/i686-w64-mingw32-g++
ln -s "$(which i686-w64-mingw32-gcc)" /usr/local/bin/i686-w64-mingw32-gcc

TARGET=win32 ./ci/win-build.sh

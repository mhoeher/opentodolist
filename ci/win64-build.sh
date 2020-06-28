#!/bin/bash

set -e

# Enable ccache:
ln -s "$(which x86_64-w64-mingw32-g++)" /usr/local/bin/x86_64-w64-mingw32-g++
ln -s "$(which x86_64-w64-mingw32-gcc)" /usr/local/bin/x86_64-w64-mingw32-gcc

TARGET=win64 ./ci/win-build.sh

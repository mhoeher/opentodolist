#!/bin/bash

set -e

# Enable ccache:
ln -s /usr/bin/ccache /usr/local/bin/x86_64-w64-mingw32-g++
ln -s /usr/bin/ccache /usr/local/bin/x86_64-w64-mingw32-gcc

TARGET=win64 ./ci/win-build.sh

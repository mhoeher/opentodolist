#!/bin/bash

set -e

# Enable ccache:
ln -s /usr/bin/ccache /usr/local/bin/i686-w64-mingw32-g++ || echo "Warning: Failed to enable ccache for g++"
ln -s /usr/bin/ccache /usr/local/bin/i686-w64-mingw32-gcc || echo "Warning: Failed to enable ccache for gcc"
export PATH=/usr/local/bin:$PATH

TARGET=win32 ./ci/win-build.sh

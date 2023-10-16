#!/bin/bash

set -e

if [ ! -d "$QT_INSTALL_ROOT" ]; then
    if [ -d "$HOME/Qt" ]; then
        QT_INSTALL_ROOT="$HOME/Qt"
    else
        echo "The variable QT_INSTALL_ROOT is not set"
        exit 1
    fi
fi
echo "Using Qt installation in $QT_INSTALL_ROOT"

if [ -z "$QT_VERSION" ]; then
    QT_VERSION=$(ls "$QT_INSTALL_ROOT" | grep -E '[0-9]+\.[0-9]+\.[0-9]+' | sort -V | tail -n1)
fi
echo "Using Qt $QT_VERSION"

if [ -n "$CI" ]; then
    curl -d install="true" -d adminlogin=admin -d adminpass=admin \
        http://nextcloud/index.php
    curl -d install="true" -d adminlogin=admin -d adminpass=admin \
            http://owncloud/index.php
    CMAKE_EXTRA_FLAGS="
        -DOPENTODOLIST_NEXTCLOUD_TEST_URL=http://admin:admin@nextcloud/
        -DOPENTODOLIST_OWNCLOUD_TEST_URL=http://admin:admin@owncloud/
    "
fi

export QT_QPA_PLATFORM=minimal

mkdir -p build-ubuntu
cd build-ubuntu

# Search for Qt libs in Qt installation explicitly. For some reason, we otherwise won't find
# all Qt libs when running tests...
# Also, add the path to the local modules we build in source, so we can find them later
# during app image build.
export LD_LIBRARY_PATH=$QT_INSTALL_ROOT/$QT_VERSION/gcc_64/lib:$PWD/modules/lib

cmake \
    -GNinja \
    -DCMAKE_PREFIX_PATH=$QT_INSTALL_ROOT/$QT_VERSION/gcc_64 \
    -DCMAKE_BUILD_TYPE=Release \
    -DOPENTODOLIST_WITH_APPIMAGE_EXTRAS=ON \
    -DOPENTODOLIST_WITH_UPDATE_SERVICE=ON \
    -DCMAKE_CXX_COMPILER_LAUNCHER=ccache \
    -DCMAKE_C_COMPILER_LAUNCHER=ccache \
    -DCMAKE_INSTALL_PREFIX=/usr \
    $CMAKE_EXTRA_FLAGS \
    ..
cmake --build .
cmake --build . --target test
rm -rf AppImageBuild
DESTDIR=$PWD/AppImageBuild cmake --build . --target install

# Remove unnecessary stuff:
rm -rf AppImageBuild/usr/{include,lib,mkspecs}

desktop-file-validate AppImageBuild/usr/share/applications/net.rpdev.OpenTodoList.desktop

# Copy OpenSSL libraries (use ones build from sources delivered with Qt):
mkdir -p AppImageBuild/usr/lib
cp \
    /usr/local/lib64/libcrypto.so.3 \
    /usr/local/lib64/libssl.so.3 \
    AppImageBuild/usr/lib

(
    cd AppImageBuild/usr/lib && \
    ln -s libcrypto.so.3 libcrypto.so && \
    ln -s libssl.so.3 libssl.so
)

export LD_LIBRARY_PATH=$PWD/AppImageBuild/usr/lib:$LD_LIBRARY_PATH

mkdir -p tools
export PATH=$PWD/tools:$PATH

if [ ! -d linuxdeploy ]; then
    curl -Lso linuxdeploy-x86_64.AppImage  https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage
    rm -rf squashfs-root
    chmod +x linuxdeploy*.AppImage
    ./linuxdeploy-x86_64.AppImage --appimage-extract
    mv squashfs-root linuxdeploy
    ln -s $PWD/linuxdeploy/AppRun tools/linuxdeploy
    rm linuxdeploy-x86_64.AppImage
fi
if [ ! -d linuxdeploy-plugin-qt ]; then
    curl -Lso linuxdeploy-plugin-qt-x86_64.AppImage https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/continuous/linuxdeploy-plugin-qt-x86_64.AppImage
    rm -rf squashfs-root
    chmod +x linuxdeploy-plugin-qt*.AppImage
    ./linuxdeploy-plugin-qt-x86_64.AppImage --appimage-extract
    mv squashfs-root linuxdeploy-plugin-qt
    ln -s $PWD/linuxdeploy-plugin-qt/AppRun tools/linuxdeploy-plugin-qt
    rm linuxdeploy-plugin-qt-x86_64.AppImage
fi

export QML_SOURCES_PATHS="$PWD/../app"

export LINUXDEPLOY_OUTPUT_VERSION=$(git describe --tags)
export QMAKE=$QT_INSTALL_ROOT/$QT_VERSION/gcc_64/bin/qmake

# Does not currently work. When using on Linux, this yields the following errors:
#
## qt.qpa.wayland: Failed to load client buffer integration: "wayland-egl"
## qt.qpa.wayland: Available client buffer integrations: QList()
## qt.qpa.wayland: No shell integration named "xdg-shell" found
## qt.qpa.wayland: No shell integration named "wl-shell" found
## qt.qpa.wayland: No shell integration named "ivi-shell" found
## qt.qpa.wayland: No shell integration named "qt-shell" found
## qt.qpa.wayland: Loading shell integration failed.
## qt.qpa.wayland: Attempted to load the following shells QList("xdg-shell", "wl-shell", "ivi-shell", "qt-shell")
## QRhiGles2: Failed to create temporary context
## QRhiGles2: Failed to create context
## Failed to create RHI (backend 2)
## Failed to initialize graphics backend for OpenGL.
## Aborted (core dumped)

# export EXTRA_PLATFORM_PLUGINS="libqwayland-generic.so;libqwayland-egl.so"

linuxdeploy --appdir AppImageBuild --plugin qt --output appimage

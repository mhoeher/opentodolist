#!/bin/bash

set -e

cd "$(dirname "$(dirname "$(readlink -f "$0")")")"

apt-get update -y
apt-get install -y \
    software-properties-common \
    cmake \
    build-essential \
    libgl1-mesa-dev \
    git \
    ccache \
    ninja-build \
    pkg-config \
    libsecret-1-dev
add-apt-repository -y ppa:beineri/opt-qt-5.15.2-focal
apt -y update
apt-get install -y \
    qt515base \
    qt515declarative \
    qt515imageformats \
    qt515quickcontrols \
    qt515quickcontrols2 \
    qt515remoteobjects \
    qt515svg \
    qt515translations \
    qt515x11extras \
    qt515tools
mkdir -p build-snapcraft
cd build-snapcraft

cmake \
    -GNinja \
    -DCMAKE_PREFIX_PATH=/opt/qt515 \
    -DQt5DBus_DIR=/opt/qt515/lib/cmake/Qt5DBus/ \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_COMPILER_LAUNCHER=ccache \
    -DCMAKE_C_COMPILER_LAUNCHER=ccache \
    -DCMAKE_INSTALL_PREFIX=/usr \
    -DCMAKE_INSTALL_RPATH=\$ORIGIN/../../$(echo /opt/qt515/lib/) \
    ..
cmake --build .
DESTDIR=$SNAPCRAFT_PART_INSTALL cmake --build . --target install

mkdir -p $SNAPCRAFT_PART_INSTALL/opt
cp -r /opt/* $SNAPCRAFT_PART_INSTALL/opt/
cp ../templates/snap/OpenTodoList-launcher $SNAPCRAFT_PART_INSTALL/usr/bin/
chmod +x $SNAPCRAFT_PART_INSTALL/usr/bin/OpenTodoList-launcher

# Install qt.conf file to point to the right Qt installation
cat > $SNAPCRAFT_PART_INSTALL/usr/bin/qt.conf <<EOF
[Paths]
Prefix = ../..$(echo /opt/qt5*/)
EOF

# Patch desktop file entry:
sed -i \
    -e 's#Icon=net.rpdev.OpenTodoList#Icon=\${SNAP}/usr/share/icons/hicolor/256x256/apps/net.rpdev.OpenTodoList.png#' \
    -e 's/Name=OpenTodoList/Name=OpenTodoList (Snap Edition)/' \
    $SNAPCRAFT_PART_INSTALL/usr/share/applications/net.rpdev.OpenTodoList.desktop

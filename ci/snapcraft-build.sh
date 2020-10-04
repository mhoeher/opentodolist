#!/bin/bash

set -e

cd "$(dirname "$(dirname "$(readlink -f "$0")")")"

apt-get update -y
apt-get install -y \
    software-properties-common \
    build-essential \
    libgl1-mesa-dev \
    git \
    qtbase5-dev
add-apt-repository -y ppa:beineri/opt-qt-5.15.0-focal
apt -y update
apt-get install -y \
    qt515base \
    qt515declarative \
    qt515imageformats \
    qt515quickcontrols2 \
    qt515remoteobjects \
    qt515svg \
    qt515translations \
    qt515x11extras \
    qt515tools
mkdir -p build-snapcraft
cd build-snapcraft
/opt/qt5*/bin/qmake \
    CONFIG+=release \
    INSTALL_PREFIX=/usr \
    QMAKE_RPATHDIR=../../$(echo /opt/qt5*/lib/) \
    ..
make -j2
make install INSTALL_ROOT=$SNAPCRAFT_PART_INSTALL

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

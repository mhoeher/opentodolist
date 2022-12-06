#!/bin/bash

set -e

cd "$(dirname "$(dirname "$(readlink -f "$0")")")"

# Extract the AppImage we previously built
rm -rf build-snap
mkdir build-snap
cd build-snap
cp ../build-ubuntu/OpenTodoList*.AppImage .
chmod +x OpenTodoList*.AppImage
./OpenTodoList*.AppImage --appimage-extract

# Move the usr folder contents into the target location:
apt-get update && apt-get install -y rsync
rsync -a ./squashfs-root/usr $SNAPCRAFT_PART_INSTALL

# Create a symlink to DRI drivers are found:
(cd $SNAPCRAFT_PART_INSTALL/usr/lib && ln -s *-linux-gnu/dri dri)

# Patch desktop file entry:
sed -i \
    -e 's#Icon=net.rpdev.OpenTodoList#Icon=\${SNAP}/usr/share/icons/hicolor/256x256/apps/net.rpdev.OpenTodoList.png#' \
    -e 's/Name=OpenTodoList/Name=OpenTodoList (Snap Edition)/' \
    $SNAPCRAFT_PART_INSTALL/usr/share/applications/net.rpdev.OpenTodoList.desktop

#!/bin/bash

set -e

PROJECT_ROOT=$(cd $(dirname $0) && cd .. && pwd)
cd $PROJECT_ROOT

# Prepare build environment, see
# http://allanmcrae.com/2015/01/replacing-makepkg-asroot/
if [ ! -d /home/build ]; then
    mkdir /home/build
    chgrp nobody /home/build
    chmod g+ws /home/build
fi

cp PKGBUILD /home/build/
chown  nobody:nobody /home/build/PKGBUILD
if [ -n "$CI_COMMIT_SHA" ]; then
    sed -i \
        -e "s/#commit=..pkgver./#commit=${CI_COMMIT_SHA}/g" \
        -e "s/release_build=yes/#release_build=yes/g" \
        /home/build/PKGBUILD
fi

# Note: We cannot use `makepkg -s`, as this requires a sudo password
# which we in turn cannot enter during CI runs. Hence, extract dependencies
# and install them before:
mkdir -p build-arch

# Install base development packages:
pacman -Sy --noconfirm base-devel

# Create Source Info:
(
    cd /home/build && \
    sudo -u nobody makepkg --printsrcinfo > $PROJECT_ROOT/build-arch/.SRCINFO \
)

# Install dependencies
pacman -Sy --noconfirm \
    $( . PKGBUILD && echo ${depends[*]} ) \
    $( . PKGBUILD && echo ${makedepends[*]} )
(
    cd /home/build && \
    sudo -u nobody makepkg \
)

cp /home/build/opentodolist-*.pkg.tar.zst build-arch

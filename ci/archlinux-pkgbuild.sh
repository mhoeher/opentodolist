#!/bin/bash

set -e

PROJECT_ROOT=$(cd $(dirname $0) && cd .. && pwd)
cd $PROJECT_ROOT

pacman -Sy --noconfirm base-devel

# Prepare build environment, see
# http://allanmcrae.com/2015/01/replacing-makepkg-asroot/
if [ ! -d /home/build ]; then
    mkdir /home/build
    chgrp nobody /home/build
    chmod g+ws /home/build
fi

cp PKGBUILD /home/build/
chown  nobody:nobody /home/build/PKGBUILD
sed -i \
    -e "s/#commit=..pkgver./#commit=${CI_COMMIT_SHA}/g" \
    -e "s/release_build=yes/#release_build=yes/g" \
    /home/build/PKGBUILD
(cd /home/build && sudo -u nobody makepkg)
mkdir -p build-arch
cp /home/build/OpenTodoList-*.pkg.tar.zst build-arch

#!/bin/bash

set -e

if [ -n "$CI" ]; then
    dnf install -y --nogpgcheck \
        flatpak \
        flatpak-builder \
        libappstream-glib
    echo "$FLATPAK_SIGNING_KEY" > flatpak-signing-key.asc
    gpg2 --import ./flatpak-signing-key.asc
    rm flatpak-signing-key.asc
fi

appstream-util validate-relax net.rpdev.OpenTodoList.appdata.xml

flatpak remote-add \
    --if-not-exists \
    flathub https://flathub.org/repo/flathub.flatpakrepo
flatpak install -y flathub org.kde.Platform//5.12
flatpak install -y flathub org.kde.Sdk//5.12

mkdir -p fp-app fp-repo
flatpak-builder \
    --ccache \
    --repo=fp-repo \
    --subject="Build of OpenTodoList `date`" \
    --gpg-sign 3D5B6355C13D18AB \
    fp-app \
    net.rpdev.OpenTodoList.json

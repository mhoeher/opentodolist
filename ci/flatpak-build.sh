#!/bin/bash

if [ -n "$CI" ]; then
    dnf install -y --nogpgcheck \
        flatpak flatpak-builder
fi


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
    fp-app \
    net.rpdev.OpenTodoList.json

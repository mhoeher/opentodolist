#!/bin/bash

set -e

if [ -n "$CI" ]; then
    echo "$FLATPAK_SIGNING_KEY" > flatpak-signing-key.asc
    gpg2 --import ./flatpak-signing-key.asc
    rm flatpak-signing-key.asc
fi

appstream-util validate-relax net.rpdev.OpenTodoList.metainfo.xml

mkdir -p fp-app fp-repo
flatpak-builder \
    --ccache \
    --repo=fp-repo \
    --subject="Build of OpenTodoList `date`" \
    --gpg-sign 3D5B6355C13D18AB \
    fp-app \
    net.rpdev.OpenTodoList.json

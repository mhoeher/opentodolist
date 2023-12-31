#!/bin/bash

set -e

if [ -n "$CI" ]; then
    # Download secret files
    export SECURE_FILES_DOWNLOAD_PATH=.secure-files
    curl --silent "https://gitlab.com/gitlab-org/incubation-engineering/mobile-devops/download-secure-files/-/raw/main/installer" | bash
    gpg2 --import $SECURE_FILES_DOWNLOAD_PATH/flatpak-signing-key.asc
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

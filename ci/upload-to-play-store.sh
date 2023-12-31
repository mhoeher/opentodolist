#!/bin/bash

set -e

SCRIPT_DIR="$(cd $(dirname "$0") && pwd)"
cd "$SCRIPT_DIR"
cd ..

export LC_ALL=en_US.UTF-8
export LANG=en_US.UTF-8

if [ -n "$CI" ]; then
    dnf install -y \
        gcc gcc-c++ ruby-devel curl ncurses which
    gem install fastlane

    export SECURE_FILES_DOWNLOAD_PATH=.secure-files
    curl --silent "https://gitlab.com/gitlab-org/incubation-engineering/mobile-devops/download-secure-files/-/raw/main/installer" | bash

    JSON_KEY=$SECURE_FILES_DOWNLOAD_PATH/opentodolist-google-cloud-service-account.json
fi

unset AAB_FILES

for file in $PWD/build-android/*.aab; do
    if [ -n "$AAB_FILES" ]; then
        AAB_FILES="$AAB_FILES,"
    fi
    AAB_FILES="$AAB_FILES$file"
done


fastlane run upload_to_play_store \
    package_name:net.rpdev.opentodolist \
    json_key:"$JSON_KEY" \
    aab_paths:"$AAB_FILES" \
    track:"internal"
